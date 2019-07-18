% ====================================================================
% Lot Sizing problem: Optimisation challenge
%
% CP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

include "global_cardinality.mzn";
include "alldifferent.mzn";  % for redundant constraint
include "at_least.mzn"; % for redundant constraint
include "at_most.mzn"; % for redundant constraint
include "regular.mzn"; % for production_order constraint and c

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
% For each order, the time period in which it is produced
array[Orders] of var Periods: production_period;
% the inventory periods that are required for the production plan 
% (i.e. the number of periods the order is completed before the due date)
array[Orders] of var 0..max(due_period): inventory_periods;
% the change cost for changing the machine setup from period p to p+1
array[1..nb_periods-1] of var 0..max(change_cost): change_cost_for_period;
% the order in which orders are produced
array[Periods] of var Orders0: production_order; 

% =============== CONSTRAINTS =========================================
                        
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

% set "production_order" to the order in which items are produced   
constraint 
    production_order[1] = production_by_order[1];  
constraint 
    forall (p in 2..nb_periods) (
      if production_by_order[p] == 0 then 
          production_order[p] = production_order[p-1]
      else 
          production_order[p] = production_by_order[p]
      endif
      )
;

constraint 
   forall(o in Orders) (
      at_least(1, production_order, o) /\
      at_most(1 + (nb_periods - nb_orders), production_order, o)
   );
   
% regular constraint for the production order: zeros at beginning allowed, but then only followed by positive numbers

%   0             (1..nbOrders)     (0..nbOrders)
%   _                   __             __
%  | |                 |  |           |  |
%  | v (1..nbOrders)   |  v   0       |  v
%  Q0  ------------->   Q1 --------->  Q2

% int: Q0 = 1;  % start state
% int: Q1 = 2;  % accepting state
% int: Q2 = 3;  % non-accepting state 
% set of int: DFA_States = 1..3;

% constraint 
%   regular(production_order,
%       3,  % number of states of the DFA
%       nb_periods, % input length to the DFA
%       transition,
%       Q0,   % start state
%       {Q1}  % accepting state(s)
%       );
      
% array[DFA_States, Periods] of DFA_States: transition =     
% array2d(DFA_States, Periods,       
%   [  if state == Q0 /\ order-1 > 0 then Q1  % first transition Q0 -> Q1
%      elseif order-1 == 0 /\ state == Q1 then Q2  % second transition Q1 -> Q2
%      else state  % otherwise, stay in the same state  
%      endif
%       |  order in Periods, state in DFA_States]
% );


% the change cost is applied when changing from one item type to another  
constraint 
   forall (p in 1..nb_periods-1) (
        change_cost_for_period[p] = change_cost[production_order[p], production_order[p+1]]
   );
   
   
% breaking symmetry: complete orders of same type in a fixed order (the ones first are produced first) 
constraint 
  forall(item_type in Items) (
     if nb_of_orders[item_type] > 1 then
        forall(k in 1..(nb_of_orders[item_type]-1)) (
            production_period[order_number(item_type, k)] < production_period[order_number(item_type, k+1)]
        )
     else true 
     endif
  );  
      
  
% returns the order number of the k-th order of item_type  
function int: order_number(Items: item_type, int: k) = 
   if item_type == 1 
     then k
   else 
      sum( [ nb_of_orders[item] | item in 1..item_type-1 ]) + k 
   endif; 


% the objective is the sum of the total change costs and the total inventory costs
var 0..(max(change_cost)*nb_orders): objective;
constraint 
   objective = sum(p in 1..nb_periods-1) (change_cost_for_period[p]) 
               + sum(o in Orders) (inventory_periods[o]) * inventory_cost;

solve :: int_search(production_by_order, first_fail, indomain_random, complete)
  minimize objective;