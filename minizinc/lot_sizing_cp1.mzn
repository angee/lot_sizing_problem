% ====================================================================
% Lot Sizing problem: Optimisation challenge
%
% CP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

include "global_cardinality.mzn";
include "exactly.mzn"; % for "no-production" variables
include "alldifferent.mzn";  % for redundant constraint
include "at_least.mzn"; % for redundant constraint
include "at_most.mzn"; % for redundant constraint

% set to "true" if you want to use the "no-production" variables
bool: use_no_production_vars = false;
% set to "true" if you want to use the "production_by_item" variables
bool: use_production_by_item_vars = true;

int: nb_item_types;   % different item types to produce
int: nb_orders;       % the total number of orders
constraint 
  assert(nb_item_types <= nb_orders, 
         "The number of item types must be greater or equal to the number of total orders.", true);
int: nb_periods;      % time periods available

% cost for inventory for one period of time
int: inventory_cost;

set of int: Orders = 1..nb_orders;
set of int: Orders0 = 0..nb_orders;
set of int: Periods = 1..nb_periods;
set of int: Items = 1..nb_item_types;
set of int: Items0 = 0..nb_item_types;

% the due date of each order
array[Orders] of Periods: due_period;
% the cost of changing from item i to item j
array[Orders0, Orders0] of int: change_cost;
% the number of orders for the item type
array[Items] of int: nb_of_orders;
% maps each order to its item type
array[Orders0] of Items0: item_type; 

% =============== VARIABLES ==========================================

% The sequence of orders that are produced
array[Periods] of var Orders0: production_by_order;
% the sequence of item types that are produced
array[Periods] of var Items0: production_by_item; % adding this var reduces search tree!
% For each order, the time period in which it is produced
array[Orders] of var Periods: production_period;
% the inventory periods that are required for the production plan 
% (i.e. the number of periods the order is completed before the due date)
array[Orders] of var 0..inventory_cost*max(due_period): inventory_periods;
% the change cost for changing the machine setup from period p to p+1
array[1..nb_periods-1] of var 0..max(change_cost): change_cost_for_period;
% is set to 0 if there is no production in the time period
array[Periods] of var 0..1: no_production;
% the order in which orders are produced
array[Periods] of var Orders0: production_order; 

% =============== CONSTRAINTS =========================================

% sets the number of times each item type has to appear in the production plan:
constraint 
  if use_production_by_item_vars then
    global_cardinality(production_by_item, 
                     [ value | value in Items0], 
                     [ 
                        if item == 0 
                            then nb_periods - nb_orders 
                        else 
                            nb_of_orders[item]
                        endif
                        | item in Items0])
  else 
     forall(p in Periods) (production_by_item[p] = 0)
  endif;
                        
% sets the number of times each order has to appear in the production plan
% each order has to be produced exactly once.
constraint 
  global_cardinality(production_by_order, 
                     [ value | value in Orders0], 
                     [ 
                        if order == 0 
                            then nb_periods - nb_orders 
                        else 
                            1
                        endif
                        | order in Orders0]);                                              

% Linking production_by_item and production_by_order variables
constraint 
 if use_production_by_item_vars then
     forall (p in Periods) (
         production_by_item[p] = item_type[production_by_order[p]]
     )
  else 
     true
  endif;

% Don't produce the order AFTER its due date
constraint 
   forall (order in Orders) (
      forall (period in Periods where due_period[order] < period) (
           production_by_order[period] != order
      )
   );         

% Linking the production_period variables with the main order variables
constraint 
   forall (order in Orders) (
       production_by_order[production_period[order]] = order
   );  
constraint alldifferent(production_period);
   
% the number of periods that inventory is necessary for each order   
constraint 
   forall(order in Orders) (  
       inventory_periods[order] = due_period[order] - production_period[order]
   );   

% setting the "no production"-variables which are needed to formulate the change cost constraint
constraint 
   if use_no_production_vars then
      forall (p in Periods) (
          ((production_by_order[p] == 0) <-> (no_production[p] = 1))
          /\ ((production_by_order[p] > 0) <-> (no_production[p] = 0) )
          /\ (exactly(nb_periods - nb_orders, no_production, 1))
          )
   else   
      forall (p in Periods) ( no_production[p] = 0)
   endif
;    

% set "production_order" to the order in which items are produced   
constraint 
    production_order[1] = production_by_order[1];  
constraint 
 if use_no_production_vars then
    forall (p in 2..nb_periods) (
      if no_production[p] == 1 then 
          production_order[p] = production_by_order[p-1]
      else 
          production_order[p] = production_by_order[p]
      endif
    )
  else
    forall (p in 2..nb_periods) (
      if production_by_order[p] == 0 then 
          production_order[p] = production_by_order[p-1]
      else 
          production_order[p] = production_by_order[p]
      endif
      )
  endif
;

constraint 
   forall(o in Orders) (
      at_least(1, production_order, o) /\
      at_most(1 + (nb_periods - nb_orders), production_order, o)
   );

% the change cost is applied when changing from one item type to another  
constraint 
   forall (p in 1..nb_periods-1) (
        change_cost_for_period[p] = change_cost[production_order[p], production_order[p+1]]
   );

% the objective is the sum of the total change costs and the total inventory costs
var 0..(max(change_cost)*nb_orders): objective;
constraint 
   objective = sum(p in 1..nb_periods-1) (change_cost_for_period[p]) 
               + sum(o in Orders) (inventory_periods[o]) * inventory_cost;

solve minimize objective;