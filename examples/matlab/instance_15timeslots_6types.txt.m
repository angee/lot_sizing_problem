% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 6 item types
%
% h - storage cost
% C - 6x6 matrix containing change-over costs between 6 different item types
% D - 15x6 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..6
h = 10;

C = [
 0  78  86  93  120  12 ;
 165  0  193  213  178  12 ;
 214  170  0  190  185  12 ;
 178  177  185  0  196  12 ;
 201  199  215  190  0  12 ;
 201  100  88  190  14  0 ;
];
D = [
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 0  0  0  0  0  0 ;
 1  0  0  0  0  0 ;
 0  1  0  0  1  0 ;
 0  0  1  0  1  1 ;
 0  0  0  0  0  0 ;
 0  1  0  0  0  0 ;
 0  0  0  1  0  0 ;
 0  0  0  0  0  1 ;
 0  0  0  0  0  0 ;
];
