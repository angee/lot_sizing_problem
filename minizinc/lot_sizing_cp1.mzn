% ====================================================================
% Lot Sizing problem: Optimisation challenge
%
% CP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

include "global_cardinality.mzn";
include "at_least.mzn";

int: nb_item_types;   % different item types to produce
int: nb_orders;       % the total number of orders
%assert(nb_item_types <= nb_orders, "The number of item types must be greater or equal to the number of total orders.");
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
%%array[Items0, Items0] of int: change_cost;
array[Orders0, Orders0] of int: change_cost;
% the number of orders for the item type
array[Items] of int: nb_of_orders;
array[Orders0] of 0..nb_item_types: item_type; 

% =============== VARIABLES ==========================================

% The sequence of orders that are produced
array[Periods] of var 0..nb_orders: production_by_order;
% the sequence of item types that are produced
array[Periods] of var 0..nb_item_types: production_by_item;
% For each order, the time period in which it is produced
array[Orders] of var Periods: production_period;
% the inventory periods that are required for the production plan 
% (i.e. the number of periods the order is completed before the due date)
array[Orders] of var 0..inventory_cost*max(due_period): inventory_periods;
% the change cost for changing the machine setup from period p to p+1
array[1..nb_periods-1] of var 0..max(change_cost): change_cost_for_period;
% is set to 0 if there is no production in the time period
array[Periods] of var 0..1: no_production;
% the order in which the orders are produced
array[Orders] of var Orders: order_of_orders;
% the order in which orders are produced
array[Periods] of var 0..nb_orders: production_order; 

% =============== CONSTRAINTS =========================================

% sets the number of times each item type has to appear in the production plan
constraint 
  global_cardinality(production_by_item, 
                     [ value | value in 0..nb_item_types], 
                     [ 
                        if item == 0 
                            then nb_periods - nb_orders 
                        else 
                            nb_of_orders[item]
                        endif
                        | item in 0..nb_item_types]);
                        
% sets the number of times each order has to appear in the production plan
constraint 
  global_cardinality(production_by_order, 
                     [ value | value in 0..nb_orders], 
                     [ 
                        if order == 0 
                            then nb_periods - nb_orders 
                        else 
                            1
                        endif
                        | order in 0..nb_orders]);                                              

% la main a la pate
% ENSICaen

% Don't produce the product AFTER its due date
constraint 
   forall (order in Orders) (
      forall (period in Periods where due_period[order] < period) (
           production_by_order[period] != order
      )
   );
   

% Linking production_by_item and production_by_order
constraint 
   forall (p in Periods) (
         production_by_item[p] = item_type[production_by_order[p]]
   );


% Linking the production_period variables with the main order variables
constraint 
   forall (order in Orders) (
       production_by_order[production_period[order]] = order
   );
   
% the number of periods that inventory is necessary for each order   
constraint 
   forall(order in Orders) (  
       inventory_periods[order] = due_period[order] - production_period[order]
   );   

% setting the no production variables
constraint 
   forall (p in Periods) (
       (production_by_order[p] == 0) <-> (no_production[p] = 1)
       );

constraint 
   forall (p in Periods) (              
      (production_by_order[p] > 0) <-> no_production[p] = 0    
   );      

% the order in which items are produced   
constraint 
    production_order[1] = production_by_order[1];  
constraint 
   forall (p in 2..nb_periods) (
      if no_production[p] == 1 then 
          production_order[p] = production_by_order[p-1]
      else 
          production_order[p] = production_by_order[p]
      endif
   );
   
constraint 
   forall (p in 1..nb_periods-1) (
        change_cost_for_period[p] = change_cost[production_order[p], production_order[p+1]]
   );

var 0..(max(change_cost)*nb_orders): objective;
constraint 
   objective = sum(p in 1..nb_periods-1) (change_cost_for_period[p]) + sum(o in Orders) (inventory_periods[o]) * inventory_cost;

solve minimize objective;