Jianan Xu (jxu147@jhu.edu)
This was an individual work.

================================
Part(b) :
We run the same experiment a couple of times, and calculated the average on the real time. Here it is:
---------------------------
Treshold  | Avg Real Time |
2097152   | 0.314         |
1048576   | 0.17875       |
524288    | 0.11775       |
262144    | 0.09475       |
131072    | 0.08075       | min
65536     | 0.0815        |
32768     | 0.0835        |
16384     | 0.08625       |
---------------------------

## 1. Below is the amount of time that my parsort program took. Let's sort threshold values according to the real time,
we get T(131072) < T(65536) < T(32768) < T(16384) < T(262144) < T(524288) < T(1048576) < T(2097152).

## 2. It was fatest when the threshold is 131072. It is reasonable. If we run a completely sequential sort, it takes 
O(n log n) time on average or in worst-case. Thus, when we increase degrees of parallelism, sequential sorting at 
the base cases of the recursion are being executed in different processes. Besides, merging is also executed in 
different non-leaf processes. The work done by different process could be scheduled by the OS kernel in parallel on 
different CPU cores.
    We can also find that, when the threshold goes smaller (<= 65536), the real time becomes larger. The reason could be
that the more processes brings more communication overhead (dividing up the work, aggregating the results back). As we 
increase the number of processes, we get diminishing returns from making the work very smaller, and increased communication
overhead. 
    Thus, when writing parallel applications, we should trade off what number of processes yields the best performance 
but minimizes overhead.

Here is the experiment result:
---------------------------------
Test run with threshold 2097152

real    0m0.312s
user    0m0.304s
sys     0m0.000s
---------------------------------
Test run with threshold 1048576

real    0m0.172s
user    0m0.292s
sys     0m0.017s
---------------------------------
Test run with threshold 524288

real    0m0.117s
user    0m0.335s
sys     0m0.010s
---------------------------------
Test run with threshold 262144

real    0m0.092s
user    0m0.405s
sys     0m0.009s
---------------------------------
Test run with threshold 131072

real    0m0.079s
user    0m0.484s
sys     0m0.046s
---------------------------------
Test run with threshold 65536

real    0m0.083s
user    0m0.490s
sys     0m0.057s
---------------------------------
Test run with threshold 32768

real    0m0.082s
user    0m0.561s
sys     0m0.037s
---------------------------------
Test run with threshold 16384

real    0m0.080s
user    0m0.560s
sys     0m0.017s