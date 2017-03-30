\ENCODING=ISO-8859-1
\Problem name: instancia multicomm-flow

Minimize
 obj: U_max
Subject To
 LinkCapm(0,2)#0:  x_0_(0,2) + x_1_(0,2) <= 1
 LinkCapm(2,3)#1:  x_0_(2,3) + x_1_(2,3) <= 1
 LinkCapm(3,4)#2:  x_0_(3,4) + x_1_(3,4) <= 1
 LinkCapm(3,8)#3:  x_0_(3,8) + x_1_(3,8) <= 1
 LinkCapm(4,5)#4:  x_0_(4,5) + x_1_(4,5) <= 1
 LinkCapm(5,0)#5:  x_0_(5,0) + x_1_(5,0) <= 1
 LinkCapm(1,6)#6:  x_0_(1,6) + x_1_(1,6) <= 1
 LinkCapm(6,7)#7:  x_0_(6,7) + x_1_(6,7) <= 1
 LinkCapm(7,8)#8:  x_0_(7,8) + x_1_(7,8) <= 1
 LinkCapm(7,4)#9:  x_0_(7,4) + x_1_(7,4) <= 1
 LinkCapm(8,9)#10: x_0_(8,9) + x_1_(8,9) <= 1
 LinkCapm(9,1)#11: x_0_(9,1) + x_1_(9,1) <= 1
 FlowConsmN2F0#12: x_0_(0,2) - x_0_(2,3)  = 0
 FlowConsmN2F1#13: x_1_(0,2) - x_1_(2,3)  = 0
 FlowConsmN3F0#14: x_0_(2,3) - x_0_(3,4) - x_0_(3,8)  = 0
 FlowConsmN3F1#15: x_1_(2,3) - x_1_(3,4) - x_1_(3,8)  = 0
 FlowConsmN4F0#16: x_0_(3,4) - x_0_(4,5) + x_0_(7,4)  = 0
 FlowConsmN4F1#17: x_1_(3,4) - x_1_(4,5) + x_1_(7,4)  = 0
 FlowConsmN5F0#18: x_0_(4,5) - x_0_(5,0)  = 0
 FlowConsmN5F1#19: x_1_(4,5) - x_1_(5,0)  = 0
 FlowConsmN6F0#20: x_0_(1,6) - x_0_(6,7)  = 0
 FlowConsmN6F1#21: x_1_(1,6) - x_1_(6,7)  = 0
 FlowConsmN7F0#22: x_0_(6,7) - x_0_(7,8) - x_0_(7,4)  = 0
 FlowConsmN7F1#23: x_1_(6,7) - x_1_(7,8) - x_1_(7,4)  = 0
 FlowConsmN8F0#24: x_0_(3,8) + x_0_(7,8) - x_0_(8,9)  = 0
 FlowConsmN8F1#25: x_1_(3,8) + x_1_(7,8) - x_1_(8,9)  = 0
 FlowConsmN9F0#26: x_0_(8,9) - x_0_(9,1)  = 0
 FlowConsmN9F1#27: x_1_(8,9) - x_1_(9,1)  = 0
 _ExitSourmF0#28:  x_0_(0,2)  = 1
 _ExitSourmF1#29:  x_1_(1,6)  = 1
 _EnterSinkmF0#30: x_0_(5,0)  = 1
 _EnterSinkmF1#31: x_1_(9,1)  = 1
End
