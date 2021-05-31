% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 5 item types
%
% h - storage cost
% C - 5x5 matrix containing change-over costs between 5 different item types
% D - 15x5 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..5
h = 10;

C = [
 0  78  86  93  120 ;
 165  0  193  213  178 ;
 214  170  0  190  185 ;
 178  177  185  0  196 ;
 201  199  215  190  0 ;
];
D = [
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
 1  0  0  0  0 ;
 0  1  0  0  1 ;
 0  0  1  0  1 ;
 0  0  0  0  0 ;
 0  1  0  0  0 ;
 0  0  0  1  0 ;
 0  0  0  0  0 ;
 0  0  0  0  0 ;
];
