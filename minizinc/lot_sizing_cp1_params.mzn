% ====================================================================
% Lot Sizing problem: Optimisation challenge
% PARAMETERS
% 
% CP Model version 1.0
% Andrea Rendl, July 2019
% ====================================================================

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
