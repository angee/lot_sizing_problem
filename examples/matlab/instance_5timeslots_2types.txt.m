% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 5 periods, 2 item types
%
% h - storage cost
% C - 2x2 matrix containing change-over costs between 2 different item types
% D - 5x2 matrix containing demand D(p,t) for period p in 1..5 and item type t in 1..2
h = 2;

C = [
 0  5 ;
 3  0 ;
];
D = [
 1  0 ;
 0  0 ;
 0  0 ;
 1  1 ;
 0  1 ;
];
