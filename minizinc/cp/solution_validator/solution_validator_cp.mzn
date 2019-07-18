% ====================================================================
% Lot Sizing problem: Optimisation challenge
% SOLUTION VALIDATOR
%
% CP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

include "lot_sizing_cp_params.mzn";

% The sequence of orders that are produced
array[Periods] of Orders0: production_by_order;
% the sequence of item types that are produced
array[Periods] of Items0: production_by_item; % adding this var reduces search tree!
% For each order, the time period in which it is produced
array[Orders] of Periods: production_period;
% the inventory periods that are required for the production plan 
% (i.e. the number of periods the order is completed before the due date)
array[Orders] of 0..inventory_cost*max(due_period): inventory_periods;
% the change cost for changing the machine setup from period p to p+1
array[1..nb_periods-1] of 0..max(change_cost): change_cost_for_period;
% is set to 0 if there is no production in the time period
array[Periods] of 0..1: no_production;
% the order in which orders are produced
array[Periods] of Orders0: production_order;
int: objective;

   
constraint 
   forall (order in Orders) (
      production_period[order] <= due_period[order] /\
      trace("Production of order " ++ show(order) ++ " (" ++ show(production_period[order]) ++ ") is before or equal its due date (" ++ show(due_period[order])++ ").\n")
   ) /\ trace("--------------------------------------------------\n");  
   
    
constraint 
   forall (order in Orders) (
      (inventory_periods[order] = due_period[order] - production_period[order] )
      /\
      trace("The inventory periods for order " ++ show(order) ++ " (" ++ show(inventory_periods[order]) ++ ") is correct.\n")
   ) /\ trace("--------------------------------------------------\n");

      
constraint         
  forall (order in Orders) (
     exists (p in Periods) (
         (production_by_order[p] = order)
         /\ trace("Order " ++ show(order) ++ " is produced.\n")
     )
  ) /\ trace("--------------------------------------------------\n");
  

% checks if the production_order variables are correct (no zeros following orders being scheduled)
% This is important because this "variable" is used to verify the change costs
function bool: correct_production_order_sequence(array[Periods] of int: production_order,
                                                 bool: zero_seen, % false at beginning
                                                 bool: order_seen, % false at beginning
                                                 int: index) =  % 1 at beginning
     if index == nb_periods+1 
        then true     
     elseif production_order[index] == 0 /\ order_seen 
        then false
     elseif production_order[index] == 0
        then correct_production_order_sequence(production_order, true, false, index+1)
     elseif production_order[index] > 0  
        then correct_production_order_sequence(production_order, zero_seen, true, index+1)                      
     else 
       trace("Ended up in unforseen state\n")
     endif                             
;

constraint 
    correct_production_order_sequence(production_order, false, false, 1) 
    /\ trace("Correct production_order variable (for checking change costs)\n");
                                                                                                                        
constraint 
   forall(p in 1..nb_periods-1) (     
      (change_cost_for_period[p] = change_cost[production_order[p], production_order[p+1]])
      /\ trace("Change cost from period " ++ show(p)  ++ "-->" ++ show(p+1) ++ " is correct (" ++ show(change_cost_for_period[p]) ++ ")\n")
   ) /\ trace("--------------------------------------------------\n");                                         
           
                           
constraint 
   (objective == sum (p in 1..nb_periods-1) (change_cost_for_period[p]) + 
                inventory_cost * ( sum (o in Orders) (inventory_periods[o]) ))
   /\ trace("Objective is correct = " ++ show(objective) ++ "\n");                                                        
   

constraint 
    trace(concat([show(production_by_item[p]-1) ++ " " | p in Periods]));      
   
solve satisfy;   