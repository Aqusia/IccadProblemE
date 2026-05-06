1. Student WEB ID: pd26s092
2. Name: 楊澤全
3. Programming Language: C++
4. Compiler: GNU g++ (-std=c++11 -O3 -Wall -Wextra -pthread)

---

5. File Description:

```
<student_id>_pa2/
    ├── src/
    │   Source code for B*-tree based SA floorplanning
    ├── bin/
    │   Executable binary (fp)
    ├── input_pa2/
    │   Input test cases (.block / .nets)
    ├── output_pa2/
    │   Output results (.rpt) and plots (.svg)
    ├── evaluator/
    │   Checker and evaluator scripts
    ├── Makefile
    │
    ├── readme.txt
    │
    ├── plot_floorplan.py
    │   Plot the floorplan
    └── report.pdf
```

Note:

* Required files for submission are mainly `bin/`, `src/`, `Makefile`, `readme.txt`, and `report.pdf`.

* Other directories are for convenience and testing.

---

6. Compilation:

Run the following commands in the project root directory:

```
make clean
make
```

The executable will be generated at:

```
bin/fp
```

---

7. Execution:

Command format:

```
./bin/fp <alpha> <input block file> <input net file> <output file>
```

Example:

```
./bin/fp 0.5 input_pa2/ami33.block input_pa2/ami33.nets output_pa2/ami33.rpt
```

You can also use the Makefile shortcut:

```
make run CASE=ami33
```

Optional variables:

* `CASE=<benchmark_name>`  
  Default: `ami33`
* `ALPHA=<value>`  
  Default: `0.5`

Example:

```
make run CASE=ami49 ALPHA=0.5
```

---

8. Checker:

Command format:

```
./evaluator/checker <input block file> <input net file> <output file> <alpha>
```

Example:

```
./evaluator/checker input_pa2/ami33.block input_pa2/ami33.nets output_pa2/ami33.rpt 0.5
```

You can also use the Makefile shortcut:

```
make check CASE=ami33 ALPHA=0.5
```

---

9. Evaluator:

Command format:

```
bash evaluator/evaluator.sh <input block file> <input net file> <output file> <alpha>
```

Example:

```
bash evaluator/evaluator.sh input_pa2/ami33.block input_pa2/ami33.nets output_pa2/ami33.rpt 0.5
```

You can also use the Makefile shortcut:

```
make eval CASE=ami33 ALPHA=0.5
```

To run and evaluate all five public cases:

```
make run_eval_all ALPHA=0.5
```

---

10. Plotting:

The project also provides a plotting helper to generate SVG floorplan figures.

Plot one case:

```
make plot CASE=ami33
```

This reads:

```
output_pa2/ami33.rpt
```

and generates:

```
output_pa2/ami33.svg
```

Plot all predefined public cases:

```
make plot_all
```

---

11. Algorithm Overview:

This program implements a **B\*-tree based fixed-outline floorplanner** using a **multi-stage SA** framework.

Main ideas:

(1) Multi-start Strategy
 
(2) Three B*-tree perturbation moves  

(3) Multi-stage SA schedule 

(4) Overflow-aware search cost and Gaussian feasible-state selection
