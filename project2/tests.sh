if false ; then  

echo "Test 1: closegroup after opengroup"
./project 1 g3 
./project 2 g3 
read x
echo "Test 2: Simple mreceive and msend"
./project 1 g1 
./project 3 g1 100
./project 4 g1 200
./project 5 g1 100 m1
read x
./project 6 g1 200
./project 2 g1

echo "Test 3: add to multiple groups"
./project 1 g2
./project 1 g1
./project 3 g1,g2 300
./project 2 g1
./project 2 g2
read x

echo "Test 4: receiver block"
./project 1 g1
./project 3 g1 100
./project 4 g1 200
./project 6 g1 200 &
sleep 1
./project 5 g1 100 m1
read x

echo "Test 5: sender overflow block"
./project 1 g1
./project 3 g1 100
./project 5 g1 100 m101
./project 5 g1 100 m102
./project 5 g1 100 m103
./project 5 g1 100 m104
./project 5 g1 100 m105
./project 5 g1 100 m106
./project 5 g1 100 m107
./project 5 g1 100 m108
./project 5 g1 100 m109
./project 5 g1 100 m110
./project 5 g1 100 m111
./project 5 g1 100 m112
./project 5 g1 100 m113
./project 5 g1 100 m114
./project 5 g1 100 m115
./project 5 g1 100 m116
./project 5 g1 100 m117
./project 5 g1 100 m118
./project 5 g1 100 m119
./project 5 g1 100 m120
./project 5 g1 100 m121
./project 5 g1 100 m122
./project 5 g1 100 m123
./project 5 g1 100 m124
./project 5 g1 100 m125
./project 5 g1 100 m126
./project 5 g1 100 m127
./project 5 g1 100 m128
./project 5 g1 100 m129
./project 5 g1 100 m130
./project 5 g1 100 m131
./project 5 g1 100 m132
./project 5 g1 100 m133
./project 5 g1 100 m134
./project 5 g1 100 m135
./project 5 g1 100 m136
./project 5 g1 100 m137
./project 5 g1 100 m138
./project 5 g1 100 m139
./project 5 g1 100 m140
./project 5 g1 100 m141
./project 5 g1 100 m142
./project 5 g1 100 m143
./project 5 g1 100 m144
./project 5 g1 100 m145
./project 5 g1 100 m146
./project 5 g1 100 m147
./project 5 g1 100 m148
./project 5 g1 100 m149
./project 5 g1 100 m150
sleep 1
./project 2 g1
read x

echo "Test 6: recovergroup"
./project 7 g1
read x

echo "Test 7: Single sender and multiple receivers "
./project 3 g1 100
./project 4 g1 500
./project 4 g1 600
./project 5 g1 100 m1
./project 6 g1 500
./project 6 g1 600
./project 2 g1
read x

echo "Test 8: receiver multiple groups messages"
./project 1 g1
./project 1 g2
./project 4 g1 100
./project 4 g2 100
./project 3 g1 200
./project 3 g2 300
./project 5 g1 200 m200g1
./project 5 g2 300 m300g2
./project 6 g1 100
./project 6 g2 100
./project 2 g1
./project 2 g2
read x

echo "Test 9: multiple senders to group"
./project 1 g1
./project 3 g1 200
./project 3 g1 300
./project 3 g1 400
./project 5 g1 200 m1200
./project 5 g1 300 m1300
./project 5 g1 400 m1400
./project 2 g1
read x

echo "Test 10: many senders, many receivers in same group"
./project 1 g1
./project 3 g1 200
./project 3 g1 300
./project 3 g1 400
./project 4 g1 500
./project 4 g1 600
./project 4 g1 700
./project 5 g1 200 m1200
./project 5 g1 300 m1300
./project 5 g1 400 m1400
./project 6 g1 500
./project 6 g1 600
./project 6 g1 700
./project 2 g1
read x

echo "Test Case 11: many senders, many receivers in different group"
./project 1 g1
./project 1 g2
./project 3 g1 200
./project 3 g2 200
./project 3 g1 300
./project 3 g2 300
./project 3 g1 400
./project 4 g1 500
./project 4 g2 500
./project 4 g1 600
./project 4 g2 600
./project 4 g1 700
./project 4 g2 700
./project 5 g1 200 m1200g1
./project 5 g2 200 m1200g2
./project 5 g1 300 m1300g1
./project 5 g2 300 m1300g2
./project 5 g1 400 m1400g1
./project 5 g2 400 m1400g2
./project 6 g1 500
./project 6 g1 600
./project 6 g1 700
./project 6 g2 500
./project 6 g2 600
./project 6 g2 700
./project 2 g1
./project 2 g2
read x
fi


echo "Test Case 12: many receives before many sends in different group"
./project 1 g1
./project 1 g2
./project 3 g1 200
./project 3 g2 200
./project 3 g1 300
./project 3 g2 300
./project 3 g1 400
./project 4 g1 500
./project 4 g2 500
./project 4 g1 600
./project 4 g2 600
./project 4 g1 700
./project 4 g2 700

./project 5 g1 200 m1200g1
./project 5 g2 200 m1200g2
./project 5 g1 300 m1300g1
./project 5 g2 300 m1300g2
./project 5 g1 400 m1400g1
./project 5 g2 400 m1400g2
read x
./project 6 g1 500 &
./project 6 g1 600 &
./project 6 g1 700 &
./project 6 g2 500 &
./project 6 g2 600 &
./project 6 g2 700 &
sleep 5
./project 2 g1
./project 2 g2