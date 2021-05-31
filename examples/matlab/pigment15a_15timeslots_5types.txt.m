% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 5 item types
%
% h - storage cost
% C - 5x5 matrix containing change-over costs between 5 different item types
% D - 15x5 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..5
h = 10;

C = [
 0  105  154  130  100 ;
 146  0  135  139  167 ;
 101  183  0  193  113 ;
 188  112  111  0  103 ;
 179  117  161  124  0 ;
];
D = [
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  1  0  0  0 ;
 0  0  0  0  0 ;
 0  0  1  0  0 ;
 1  0  0  1  0 ;
 0  0  0  0  1 ;
 0  0  0  0  0 ;
 0  0  0  1  0 ;
 0  1  1  0  1 ;
 0  0  0  0  0 ;
 1  0  1  0  0 ;
 0  1  0  1  1 ;
 0  0  0  0  0 ;
];
