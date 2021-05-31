% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 5 item types
%
% h - storage cost
% C - 5x5 matrix containing change-over costs between 5 different item types
% D - 15x5 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..5
h = 10;

C = [
 0  128  117  197  178 ;
 125  0  102  146  175 ;
 179  191  0  190  161 ;
 156  139  119  0  189 ;
 109  121  111  187  0 ;
];
D = [
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 1  0  0  0  0 ;
 0  0  1  0  0 ;
 0  0  0  0  0 ;
 1  0  0  0  1 ;
 0  1  0  0  0 ;
 1  0  0  1  0 ;
 0  0  0  0  0 ;
 0  1  0  0  0 ;
 0  0  1  1  0 ;
 1  1  0  0  1 ;
 0  0  0  0  0 ;
];
