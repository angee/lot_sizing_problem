% ====================================================================
% Discrete Lot Sizing problem
%
% Mixed Integer Programming model, Satalia Optimisation challenge
%
% CSPlib Problem 58: http://www.csplib.org/Problems/prob058/
% MIT License
%
% Copyright (c) 2019 Andrea Rendl-Pitrey, Satalia
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
% 
% Author: Andrea Rendl, July 2019
% ====================================================================

int: nb_item_types;   % different item types to produce
int: nb_orders;       % the total number of orders
constraint 
  assert(nb_item_types <= nb_orders, 
         "The number of item types must be greater or equal to the number of total orders.", true);
int: nb_periods;      % time periods available
int: idle_periods = nb_periods - nb_orders;

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

% main variables: 1 if order o is produced in period p, 0 otherwise
array[Orders, Periods] of var 0..1: production_plan;

% helper variables: 1 if something is produced in period p, 0 otherwise
array[Periods] of var 0..1: has_production;

% helper variables: change costs that apply from order o1 to order o2 in period p   
array[Orders, Orders, 1..nb_periods-1] of var 0..max(change_cost): total_change_costs; 

% =============== CONSTRAINTS =========================================

% produce each order exactly once
constraint 
  forall(o in Orders) (
     sum (p in Periods) (production_plan[o,p]) = 1
  );


% produce no more than one order in each period
constraint 
  forall(p in Periods) (
     sum (o in Orders) (production_plan[o,p]) <= 1
  );


% produce the order before its due date
constraint 
   forall(o in Orders) (
       due_period[o] >= sum (p in Periods) (production_plan[o,p]*p)
   );        
  

% Linking the production_plan vars with the helper has_production vars
constraint
   forall (p in Periods) (
      has_production[p] = sum(o in Orders) (production_plan[o,p])
);    
  

% Setting the change costs when switching between item types between two periods
constraint    
   forall (k in 1..idle_periods+1) (
     forall(p in 1..nb_periods-k) (
        forall (o1,o2 in Orders) (
        
           total_change_costs[o1,o2, p] >=
             ( -k 
               + production_plan[o1,p]         % o1 produced at period p  
               + sum(j in 1..k-1) 
                    (1 - has_production[p+j])  % k-1 potential idle periods                 
                    
               + production_plan[o2,p+k]       % o2 produced at period p+k
               ) 
                  * (change_cost[o1,o2])  
   )))
;
 
  
% =============== OBJECTIVE =====================================================

solve minimize
               ( sum(p in 1..nb_periods-1, o1 in Orders, o2 in Orders) 
                     (total_change_costs[o1, o2, p]) )                
               +  sum(o in Orders) 
                  (inventory_cost * (due_period[o] 
                                       - sum (p in Periods) (production_plan[o,p]*p)));

                                    
% =============== OUTPUT =======================================================

% helper function for output: returns the order number for the given period
function int: get_order(int: period) = 
   if exists (o in Orders) (fix(production_plan[o,period] == 1)) 
      then sum(o in Orders) ( fix(production_plan[o,period]) * o) % returns order number
   else 0
   endif;

output
["production = [" ++ concat([ show(get_order(p)) ++ ", " 
                           | p in Periods]) ++ "]" ]                   
++
["\nobjective = " ++ show((sum(p in 1..nb_periods-1, o1 in Orders, o2 in Orders) 
                                 (fix(total_change_costs[o1, o2, p]))) 
                           +  sum(o in Orders) 
                                  (inventory_cost * (due_period[o] 
                                       - sum (p in Periods) (fix(production_plan[o,p])*p)))
                           ) ++ "\n"]
; 

                                                                                                                                        