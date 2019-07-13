% ====================================================================
% Lot Sizing problem: Optimisation challenge
%
% MIP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

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

array[Orders, Periods] of var 0..1: production_plan;
array[Periods] of var 0..1: has_production;

% =============== CONSTRAINTS =========================================

% produce each order exactly once
constraint 
  forall(o in Orders) (
     sum (p in Periods) (production_plan[o,p]) = 1
  );


% produce exactly one order in each period
constraint 
  forall(p in Periods) (
     sum (o in Orders) (production_plan[o,p]) <= 1
  );

% produce the order before its due date
constraint 
   forall(o in Orders) (
       due_period[o] >= sum (p in Periods) (production_plan[o,p]*p)
   );
   
% inventory costs
%array[Orders] of var 0..nb_periods: inventory_periods;
%constraint 
%   forall (o in Orders) (
%      due_period[o] - sum (p in Periods) (production_plan[o,p]*p)  = inventory_periods[o]
%   );   
      
% change costs that are directly after one another
%array[1..nb_periods-1] of var 0..max(change_cost): total_change_costs;   
array[Orders, Orders, 1..nb_periods-1] of var 0..max(change_cost): total_change_costs;   


% switching between item types between two periods
constraint    
   forall(p in 1..nb_periods-1) (
     forall (o1,o2 in Orders) (
   total_change_costs[o1,o2, p] >=
     ( -1 + production_plan[o1,p] + production_plan[o2,p+1]) * (change_cost[o1,o2])  
   ))
;

constraint    
   forall(p in 1..nb_periods-2, o1 in Orders, o2 in Orders) (
   total_change_costs[o1,o2,p] >=
     ( -2 + production_plan[o1,p] + 1 - has_production[p+1] + production_plan[o2,p+2]) * (change_cost[o1,o2])  
     )
;  
 
 % switching between item types between two periods and 1 idle period 
constraint    
   forall(p in 1..nb_periods-3, o1 in Orders, o2 in Orders) (
   total_change_costs[o1, o2, p] >=
      (-3 + production_plan[o1,p] + (1 - has_production[p+1]) + (1 - has_production[p+2]) + production_plan[o2,p+3]) * (change_cost[o1,o2])
      )
;  

constraint    
   forall(p in 1..nb_periods-4, o1 in Orders, o2 in Orders) (
   total_change_costs[o1, o2, p] >=
      (-4 + production_plan[o1,p] + (1 - has_production[p+1]) + (1 - has_production[p+2]) +  (1 - has_production[p+3]) + production_plan[o2,p+4]) * (change_cost[o1,o2])
      ) 
;  
  
constraint
forall (p in Periods) (
  has_production[p] = sum(o in Orders) (production_plan[o,p])
);  
  

solve minimize
               ( sum(p in 1..nb_periods-1, o1 in Orders, o2 in Orders) 
                     (total_change_costs[o1, o2, p]) ) 
               % + inventory_cost * sum(o in Orders) (inventory_periods[o]);
               +  sum(o in Orders) 
                  (inventory_cost * (due_period[o] - sum (p in Periods) (production_plan[o,p]*p)));
                  

function int: get_order(int: period) = 
   if exists (o in Orders) (fix(production_plan[o,period] == 1)) 
      then sum(o in Orders) ( fix(production_plan[o,period]) * o) % returns order number
   else 0
   endif;

output
["production_plan = [" ++ concat([ show(get_order(p)) ++ ", " 
                           | p in Periods]) ++ "]" ]
++
["\n\nproduction_plan_array = [\n" ++ concat([ show(fix(production_plan[o,p])) ++ ", " ++
                                       if p == nb_periods then "\n"
                                       else "" endif
                                    | o in Orders, p in Periods]) ++ "]" ]
%++
%["\n\ninventory_periods = " ++ show(inventory_periods) ++ "\n"] 
++
%["\nchange_costs = " ++ show(total_change_costs) ++ "\n"] 
%++
["\nhas_production = " ++ show(has_production) ++ "\n"] 
++
["\nobjective = " ++ show((sum(p in 1..nb_periods-1, o1 in Orders, o2 in Orders) (fix(total_change_costs[o1, o2, p]))) 
                +  sum(o in Orders) 
                  (inventory_cost * (due_period[o] - sum (p in Periods) (fix(production_plan[o,p])*p)))) ++ "\n"]
; 

                                                                                                                                        