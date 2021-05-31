% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 8 item types
%
% h - storage cost
% C - 8x8 matrix containing change-over costs between 8 different item types
% D - 15x8 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..8
h = 10;

C = [
 0  149  134  110  137  191  104  192 ;
 131  0  139  192  118  117  161  132 ;
 128  115  0  195  196  174  127  144 ;
 134  154  119  0  165  118  163  109 ;
 169  118  117  140  0  166  174  116 ;
 134  153  139  158  159  0  100  170 ;
 170  199  185  193  143  146  0  186 ;
 162  101  130  115  193  190  150  0 ;
];
D = [
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  1  1  0  0  0 ;
 0  1  0  0  0  0  0  1 ;
 0  0  1  0  0  1  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  1  0  0 ;
 0  0  1  1  0  0  1  1 ;
 1  0  0  0  1  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
];
