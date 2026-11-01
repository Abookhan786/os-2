# Top 35 Famous Shell Programming Questions & IPC C Labs (OS Lab)

A comprehensive compilation of the **top 35 most frequently asked Shell Scripting (Bash) programs** and **Inter-Process Communication (IPC) C programs** for Operating Systems laboratory exams, practical viva, and technical interviews.

Each problem includes:
- **Problem Statement**
- **Core Concepts Covered**
- **Complete, Well-Commented Bash Script**
- **Sample Input & Output**

---

## 📋 Table of Contents

### Core Shell Scripting
1. [Even or Odd Checker](#1-check-whether-a-number-is-even-or-odd)
2. [Largest of Three Numbers](#2-find-the-largest-among-three-numbers)
3. [Leap Year Checker](#3-check-whether-a-year-is-a-leap-year)
4. [Factorial of a Number](#4-factorial-of-a-given-number)
5. [Fibonacci Series Generator](#5-generate-fibonacci-series-up-to-n-terms)
6. [Prime Number Checker](#6-check-whether-a-number-is-prime)
7. [Reverse a Number & Sum of Digits](#7-reverse-a-number-and-find-sum-of-digits)
8. [Palindrome Checker (String & Number)](#8-check-whether-a-string-or-number-is-a-palindrome)
9. [Armstrong Number Checker](#9-check-whether-a-number-is-an-armstrong-number)
10. [Menu-Driven Arithmetic Calculator](#10-menu-driven-calculator-using-case)
11. [File Permissions & Attributes Checker](#11-check-file-existence-attributes-and-permissions)
12. [Count Lines, Words, and Characters in a File](#12-count-lines-words-and-characters-in-a-file)
13. [Student Grade Evaluation](#13-student-grade-evaluation)
14. [Search for a Pattern/String in a File](#14-search-for-a-pattern-in-a-file)
15. [File Copy with Overwrite Confirmation](#15-copy-file-with-overwrite-confirmation)
16. [Multiplication Table Generator](#16-print-multiplication-table-of-a-number)
17. [Temperature Converter (Celsius & Fahrenheit)](#17-temperature-conversion-celsius-and-fahrenheit)
18. [GCD and LCM of Two Numbers](#18-find-gcd-and-lcm-of-two-numbers)
19. [Sort an Array of Numbers (Bubble Sort)](#19-sort-an-array-of-numbers-in-ascending-order)
20. [System Information & Health Dashboard](#20-display-system-information-dashboard)
21. [Check User Login Status](#21-check-if-a-specific-user-is-logged-in)
22. [Find and Delete Empty Files](#22-find-and-delete-all-empty-zero-byte-files-in-a-directory)
23. [Compare Two Files for Differences](#23-compare-two-files-and-check-if-identical)
24. [Find Files Modified in the Last N Days](#24-find-files-modified-in-the-last-n-days-or-exceeding-a-size)
25. [Automated Timestamped Directory Backup](#25-automated-timestamped-directory-backup-and-compression)

### Advanced Exam Problems (Arrays, Strings & Math)
26. [Sum of Elements at Odd and Even Indices in an Array](#26-sum-of-elements-at-odd-and-even-indices-in-an-array)
27. [Sum of Even and Odd Digits in a Number](#27-sum-of-even-and-odd-digits-in-a-number)
28. [Second Largest Element in an Array](#28-find-the-second-largest-element-in-an-array)
29. [Linear Search & Binary Search in Array](#29-linear-search-and-binary-search-in-an-array)
30. [Count Occurrence / Frequency of Each Element in an Array](#30-count-frequency-of-each-element-in-an-array)
31. [Matrix Addition in Shell Script](#31-matrix-addition-2d-matrix-in-shell-script)
32. [Pyramid and Number Triangle Patterns](#32-pyramid-and-number-triangle-patterns)
33. [String Character, Vowel & Word Counter](#33-count-vowels-consonants-digits-and-spaces-in-a-string)
34. [Check if Two Strings are Anagrams](#34-check-if-two-strings-are-anagrams)
35. [Decimal to Binary and Binary to Decimal Conversion](#35-decimal-to-binary-and-binary-to-decimal-conversion)

---

## ⚡ Famous IPC (Parent-Child) C Programs

This repository also contains production-ready C implementations for the most famous **Inter-Process Communication (IPC)** parent-child problems across UNIX Pipes, Shared Memory, Message Queues, Semaphores, and Signals:

| Program File | IPC Mechanism | Description | Compilation Command |
|---|---|---|---|
| [`ipc_pipe_echo_transform.c`](ipc_pipe_echo_transform.c) | **UNIX Pipes (Two-Way)** | Bidirectional parent-child string transformation (case toggle & reverse) | `gcc -Wall -o ipc_pipe_echo_transform ipc_pipe_echo_transform.c` |
| [`ipc_string_char_counter.c`](ipc_string_char_counter.c) | **UNIX Pipes (Two-Way)** | Parent sends string; Child counts vowels, consonants, digits, spaces, words & returns stats struct | `gcc -Wall -o ipc_string_char_counter ipc_string_char_counter.c` |
| [`ipc_producer_consumer_shm.c`](ipc_producer_consumer_shm.c) | **Shared Memory + POSIX Semaphores** | Classic Bounded-Buffer Producer (Parent) / Consumer (Child) with `mutex`, `empty`, `full` | `gcc -Wall -o ipc_producer_consumer_shm ipc_producer_consumer_shm.c -lpthread` |
| [`ipc_collatz_pipe.c`](ipc_collatz_pipe.c) | **UNIX Pipe** | Silberschatz Problem 3.21: Child calculates Collatz ($3n+1$) sequence, parent reads & prints | `gcc -Wall -o ipc_collatz_pipe ipc_collatz_pipe.c` |
| [`ipc_fib_prime_split.c`](ipc_fib_prime_split.c) | **Dual UNIX Pipes** | Multi-child pipeline: Child 1 generates Fibonacci, Child 2 filters Primes, Parent aggregates | `gcc -Wall -o ipc_fib_prime_split ipc_fib_prime_split.c` |
| [`ipc_msg_queue_calc.c`](ipc_msg_queue_calc.c) | **System V Message Queues** | Request-Response math server: Parent requests arithmetic operations, Child computes & replies | `gcc -Wall -o ipc_msg_queue_calc ipc_msg_queue_calc.c` |
| [`ipc_signals_ping_pong.c`](ipc_signals_ping_pong.c) | **UNIX Signals (`SIGUSR1`, `SIGUSR2`)** | Process turn-taking ping-pong rally using signals and `pause()` / `sigsuspend()` | `gcc -Wall -o ipc_signals_ping_pong ipc_signals_ping_pong.c` |
| [`ipc_array_sort_shm.c`](ipc_array_sort_shm.c) | **System V Shared Memory** | Parent shares unsorted array; Child sorts & calculates median directly in memory | `gcc -Wall -o ipc_array_sort_shm ipc_array_sort_shm.c` |
| [`avg_min_max.c`](avg_min_max.c) | **Shared Memory (Multi-Child)** | Parent forks 3 children to compute average, maximum, and minimum concurrently | `gcc -Wall -o avg_min_max avg_min_max.c` |
| [`ipc_multigame_pipes.c`](ipc_multigame_pipes.c) | **UNIX Pipes** | John & Michael turn-based number multiplication race game | `gcc -Wall -o ipc_multigame_pipes ipc_multigame_pipes.c` |
| [`ipc_multigame_sm.c`](ipc_multigame_sm.c) | **Shared Memory + Semaphores** | Turn-based multiplication race synchronized with semaphores | `gcc -Wall -o ipc_multigame_sm ipc_multigame_sm.c -lpthread` |

---

## 🚀 How to Run Bash Scripts

1. **Create the script file**:
   ```bash
   nano script.sh
   # or touch script.sh && vim script.sh
   ```
2. **Grant execution permissions**:
   ```bash
   chmod +x script.sh
   ```
3. **Execute the script**:
   ```bash
   ./script.sh
   # or run directly with bash:
   bash script.sh
   ```

---

### 1. Check Whether a Number is Even or Odd

- **Concepts**: Arithmetic expansion `$((...))`, modulo operator `%`, `if-else` condition.

```bash
#!/bin/bash
# Description: Checks if an input integer is even or odd

read -p "Enter an integer: " num

if [ $((num % 2)) -eq 0 ]; then
    echo "$num is Even."
else
    echo "$num is Odd."
fi
```

**Sample Output:**
```text
Enter an integer: 17
17 is Odd.
```

---

### 2. Find the Largest Among Three Numbers

- **Concepts**: Relational operators (`-gt`, `-ge`), logical AND (`&&` or `-a`).

```bash
#!/bin/bash
# Description: Finds the maximum of three numbers

read -p "Enter first number: " a
read -p "Enter second number: " b
read -p "Enter third number: " c

if [ "$a" -ge "$b" ] && [ "$a" -ge "$c" ]; then
    largest=$a
elif [ "$b" -ge "$a" ] && [ "$b" -ge "$c" ]; then
    largest=$b
else
    largest=$c
fi

echo "The largest number is: $largest"
```

**Sample Output:**
```text
Enter first number: 45
Enter second number: 89
Enter third number: 32
The largest number is: 89
```

---

### 3. Check Whether a Year is a Leap Year

- **Concepts**: Compound leap year rule: `(year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)`.

```bash
#!/bin/bash
# Description: Determines whether a year is a leap year

read -p "Enter a year (YYYY): " year

if [ $((year % 400)) -eq 0 ] || ([ $((year % 4)) -eq 0 ] && [ $((year % 100)) -ne 0 ]); then
    echo "$year is a Leap Year."
else
    echo "$year is NOT a Leap Year."
fi
```

**Sample Output:**
```text
Enter a year (YYYY): 2024
2024 is a Leap Year.
```

---

### 4. Factorial of a Given Number

- **Concepts**: `while` loop, accumulator pattern, non-negative integer check.

```bash
#!/bin/bash
# Description: Computes the factorial of n (n!)

read -p "Enter a non-negative integer: " n

if [ "$n" -lt 0 ]; then
    echo "Factorial is not defined for negative numbers."
    exit 1
fi

fact=1
temp=$n

while [ "$temp" -gt 1 ]; do
    fact=$((fact * temp))
    temp=$((temp - 1))
done

echo "Factorial of $n is: $fact"
```

**Sample Output:**
```text
Enter a non-negative integer: 5
Factorial of 5 is: 120
```

---

### 5. Generate Fibonacci Series Up to N Terms

- **Concepts**: Iterative variable swap, sequence generation, loop counters.

```bash
#!/bin/bash
# Description: Generates first N terms of Fibonacci sequence

read -p "Enter number of terms (N): " n

a=0
b=1

echo "Fibonacci series up to $n terms:"
for (( i=0; i<n; i++ )); do
    echo -n "$a "
    fn=$((a + b))
    a=$b
    b=$fn
done
echo ""
```

**Sample Output:**
```text
Enter number of terms (N): 7
Fibonacci series up to 7 terms:
0 1 1 2 3 5 8 
```

---

### 6. Check Whether a Number is Prime

- **Concepts**: Primality test, checking divisors up to `n / 2`, flag variable.

```bash
#!/bin/bash
# Description: Checks if an input number is prime

read -p "Enter a positive integer: " num

if [ "$num" -le 1 ]; then
    echo "$num is neither prime nor composite."
    exit 0
fi

is_prime=1
for (( i=2; i*i<=num; i++ )); do
    if [ $((num % i)) -eq 0 ]; then
        is_prime=0
        break
    fi
done

if [ "$is_prime" -eq 1 ]; then
    echo "$num is a Prime Number."
else
    echo "$num is NOT a Prime Number."
fi
```

**Sample Output:**
```text
Enter a positive integer: 29
29 is a Prime Number.
```

---

### 7. Reverse a Number and Find Sum of Digits

- **Concepts**: Modulo arithmetic (`% 10`), integer division (`/ 10`), string/number manipulation.

```bash
#!/bin/bash
# Description: Reverses a number and calculates sum of its digits

read -p "Enter a number: " num

temp=$num
rev=0
sum=0

while [ "$temp" -gt 0 ]; do
    rem=$((temp % 10))
    rev=$((rev * 10 + rem))
    sum=$((sum + rem))
    temp=$((temp / 10))
done

echo "Reversed Number : $rev"
echo "Sum of Digits   : $sum"
```

**Sample Output:**
```text
Enter a number: 12345
Reversed Number : 54321
Sum of Digits   : 15
```

---

### 8. Check Whether a String or Number is a Palindrome

- **Concepts**: String reversal using `rev` or substring indexing, equality check.

```bash
#!/bin/bash
# Description: Tests if a string or number reads the same forwards and backwards

read -p "Enter a string or number: " input

# Reverse using the rev utility
reversed=$(echo "$input" | rev)

if [ "$input" = "$reversed" ]; then
    echo "\"$input\" is a Palindrome."
else
    echo "\"$input\" is NOT a Palindrome."
fi
```

**Sample Output:**
```text
Enter a string or number: madam
"madam" is a Palindrome.
```

---

### 9. Check Whether a Number is an Armstrong Number

- **Concepts**: Number length calculation, digit power sum evaluation (e.g., $153 = 1^3 + 5^3 + 3^3$).

```bash
#!/bin/bash
# Description: Checks if a number is an Armstrong number (Narcissistic number)

read -p "Enter a positive integer: " num

len=${#num}
temp=$num
sum=0

while [ "$temp" -gt 0 ]; do
    digit=$((temp % 10))
    power=$((digit ** len))
    sum=$((sum + power))
    temp=$((temp / 10))
done

if [ "$sum" -eq "$num" ]; then
    echo "$num is an Armstrong number."
else
    echo "$num is NOT an Armstrong number."
fi
```

**Sample Output:**
```text
Enter a positive integer: 153
153 is an Armstrong number.
```

---

### 10. Menu-Driven Calculator Using `case`

- **Concepts**: Infinite loop with exit condition, `case ... esac`, arithmetic functions.

```bash
#!/bin/bash
# Description: Menu-driven calculator performing basic operations

while true; do
    echo "============================="
    echo "       MENU CALCULATOR       "
    echo "============================="
    echo "1. Addition"
    echo "2. Subtraction"
    echo "3. Multiplication"
    echo "4. Division"
    echo "5. Modulus"
    echo "6. Exit"
    read -p "Choose an option [1-6]: " choice

    if [ "$choice" -eq 6 ]; then
        echo "Exiting calculator. Goodbye!"
        break
    fi

    read -p "Enter first operand: " num1
    read -p "Enter second operand: " num2

    case $choice in
        1) res=$((num1 + num2)); echo "Result: $num1 + $num2 = $res" ;;
        2) res=$((num1 - num2)); echo "Result: $num1 - $num2 = $res" ;;
        3) res=$((num1 * num2)); echo "Result: $num1 * $num2 = $res" ;;
        4)
            if [ "$num2" -eq 0 ]; then
                echo "Error: Division by zero is undefined."
            else
                res=$(echo "scale=2; $num1 / $num2" | bc)
                echo "Result: $num1 / $num2 = $res"
            fi
            ;;
        5)
            if [ "$num2" -eq 0 ]; then
                echo "Error: Modulo by zero is undefined."
            else
                res=$((num1 % num2))
                echo "Result: $num1 % $num2 = $res"
            fi
            ;;
        *) echo "Invalid option! Please select 1 through 6." ;;
    esac
    echo ""
done
```

---

### 11. Check File Existence, Attributes, and Permissions

- **Concepts**: File test operators (`-e`, `-f`, `-d`, `-r`, `-w`, `-x`, `-s`).

```bash
#!/bin/bash
# Description: Inspects a file/directory path and reports its attributes and permissions

read -p "Enter path to file or directory: " target

if [ ! -e "$target" ]; then
    echo "Error: Path '$target' does not exist."
    exit 1
fi

echo "--- Properties of '$target' ---"
if [ -f "$target" ]; then
    echo "Type       : Regular File"
    echo "Size       : $(wc -c < "$target") bytes"
elif [ -d "$target" ]; then
    echo "Type       : Directory"
fi

[ -r "$target" ] && echo "Read       : YES" || echo "Read       : NO"
[ -w "$target" ] && echo "Write      : YES" || echo "Write      : NO"
[ -x "$target" ] && echo "Execute    : YES" || echo "Execute    : NO"
[ -s "$target" ] && echo "Empty      : NO"  || echo "Empty      : YES (0 bytes)"
```

**Sample Output:**
```text
Enter path to file or directory: /etc/passwd
--- Properties of '/etc/passwd' ---
Type       : Regular File
Size       : 2842 bytes
Read       : YES
Write      : NO
Execute    : NO
Empty      : NO
```

---

### 12. Count Lines, Words, and Characters in a File

- **Concepts**: Reading files line by line using `read -r`, string length counting, word extraction.

```bash
#!/bin/bash
# Description: Counts lines, words, and characters in a file without relying solely on wc

read -p "Enter filename: " filename

if [ ! -f "$filename" ]; then
    echo "Error: File '$filename' does not exist."
    exit 1
fi

lines=0
words=0
chars=0

while IFS= read -r line || [ -n "$line" ]; do
    lines=$((lines + 1))
    
    # Character count includes line length + 1 newline character
    chars=$((chars + ${#line} + 1))
    
    # Word count in the current line
    for w in $line; do
        words=$((words + 1))
    done
done < "$filename"

echo "File Statistics for '$filename':"
echo "Lines      : $lines"
echo "Words      : $words"
echo "Characters : $chars"
```

---

### 13. Student Grade Evaluation

- **Concepts**: Multiple condition branching with `elif`, input validation, grading boundaries.

```bash
#!/bin/bash
# Description: Accepts marks of 5 subjects, computes percentage and assigns grades

echo "Enter marks for 5 subjects (0 - 100):"
total=0

for i in {1..5}; do
    read -p "Subject $i: " mark
    if [ "$mark" -lt 0 ] || [ "$mark" -gt 100 ]; then
        echo "Invalid mark. Must be between 0 and 100."
        exit 1
    fi
    total=$((total + mark))
done

percentage=$((total / 5))

echo "----------------------------"
echo "Total Marks : $total / 500"
echo "Percentage  : $percentage%"

if [ "$percentage" -ge 90 ]; then
    grade="A+ (Distinction)"
elif [ "$percentage" -ge 80 ]; then
    grade="A (First Class with Distinction)"
elif [ "$percentage" -ge 70 ]; then
    grade="B (First Class)"
elif [ "$percentage" -ge 60 ]; then
    grade="C (Second Class)"
elif [ "$percentage" -ge 50 ]; then
    grade="D (Pass)"
else
    grade="F (Fail)"
fi

echo "Grade       : $grade"
```

---

### 14. Search for a Pattern in a File

- **Concepts**: Command line arguments (`$1`, `$2`), `grep`, checking command exit status `$?`.

```bash
#!/bin/bash
# Description: Searches for a pattern inside a file and displays matching lines & count
# Usage: ./search_pattern.sh <pattern> <filename>

if [ $# -ne 2 ]; then
    echo "Usage: $0 <pattern> <filename>"
    exit 1
fi

pattern="$1"
filename="$2"

if [ ! -f "$filename" ]; then
    echo "Error: File '$filename' not found."
    exit 1
fi

echo "Searching for '$pattern' in '$filename'..."
grep -n --color=auto "$pattern" "$filename"

if [ $? -eq 0 ]; then
    count=$(grep -c "$pattern" "$filename")
    echo "Found $count occurrence(s)."
else
    echo "Pattern '$pattern' was not found in '$filename'."
fi
```

---

### 15. Copy File with Overwrite Confirmation

- **Concepts**: Interactive prompts, checking file collisions, conditional copying.

```bash
#!/bin/bash
# Description: Copies source file to destination; warns before overwriting existing files

if [ $# -ne 2 ]; then
    echo "Usage: $0 <source_file> <destination_file>"
    exit 1
fi

src="$1"
dst="$2"

if [ ! -f "$src" ]; then
    echo "Error: Source file '$src' does not exist."
    exit 1
fi

if [ -f "$dst" ]; then
    read -p "Destination file '$dst' already exists. Overwrite? (y/n): " ans
    if [ "$ans" != "y" ] && [ "$ans" != "Y" ]; then
        echo "Copy cancelled by user."
        exit 0
    fi
fi

cp "$src" "$dst"
echo "File successfully copied from '$src' to '$dst'."
```

---

### 16. Print Multiplication Table of a Number

- **Concepts**: `for` loop iteration, formatted output with `printf` or `echo`.

```bash
#!/bin/bash
# Description: Displays multiplication table up to a specified limit

read -p "Enter number: " num
read -p "Enter range (e.g. 10 or 12): " limit

echo "Multiplication Table for $num:"
for (( i=1; i<=limit; i++ )); do
    echo "$num x $i = $((num * i))"
done
```

---

### 17. Temperature Conversion (Celsius and Fahrenheit)

- **Concepts**: Floating point computation via `bc`, formula translation ($F = C \times \frac{9}{5} + 32$).

```bash
#!/bin/bash
# Description: Converts between Celsius and Fahrenheit

echo "1. Celsius to Fahrenheit"
echo "2. Fahrenheit to Celsius"
read -p "Select choice [1 or 2]: " choice

case $choice in
    1)
        read -p "Enter temperature in Celsius: " c
        f=$(echo "scale=2; ($c * 9 / 5) + 32" | bc)
        echo "$c °C = $f °F"
        ;;
    2)
        read -p "Enter temperature in Fahrenheit: " f
        c=$(echo "scale=2; ($f - 32) * 5 / 9" | bc)
        echo "$f °F = $c °C"
        ;;
    *)
        echo "Invalid selection."
        ;;
esac
```

---

### 18. Find GCD and LCM of Two Numbers

- **Concepts**: Euclidean algorithm for Greatest Common Divisor; $\text{LCM}(a, b) = \frac{a \times b}{\text{GCD}(a, b)}$.

```bash
#!/bin/bash
# Description: Finds GCD (HCF) and LCM of two integers

read -p "Enter first number: " a
read -p "Enter second number: " b

x=$a
y=$b

# Euclidean algorithm
while [ "$y" -ne 0 ]; do
    rem=$((x % y))
    x=$y
    y=$rem
done

gcd=$x
lcm=$(( (a * b) / gcd ))

echo "GCD of ($a, $b) = $gcd"
echo "LCM of ($a, $b) = $lcm"
```

---

### 19. Sort an Array of Numbers in Ascending Order

- **Concepts**: Bash arrays `${arr[@]}`, array indexing, nested loops (Bubble Sort algorithm).

```bash
#!/bin/bash
# Description: Reads numbers into an array and sorts them using Bubble Sort

read -p "Enter numbers separated by spaces: " -a arr
n=${#arr[@]}

echo "Original Array: ${arr[*]}"

for (( i=0; i<n-1; i++ )); do
    for (( j=0; j<n-i-1; j++ )); do
        if [ "${arr[j]}" -gt "${arr[j+1]}" ]; then
            # Swap elements
            temp=${arr[j]}
            arr[j]=${arr[j+1]}
            arr[j+1]=$temp
        fi
    done
done

echo "Sorted Array  : ${arr[*]}"
```

**Sample Output:**
```text
Enter numbers separated by spaces: 54 12 90 3 27
Original Array: 54 12 90 3 27
Sorted Array  : 3 12 27 54 90
```

---

### 20. Display System Information Dashboard

- **Concepts**: Unix monitoring commands (`uname`, `whoami`, `uptime`, `free`, `df`, `date`).

```bash
#!/bin/bash
# Description: System diagnostic and health check overview

echo "============================================="
echo "         SYSTEM INFORMATION DASHBOARD        "
echo "============================================="
echo "Current Date & Time : $(date)"
echo "Current User        : $(whoami)"
echo "Hostname            : $(hostname)"
echo "Operating System    : $(uname -s)"
echo "Kernel Version      : $(uname -r)"
echo "System Uptime       : $(uptime -p 2>/dev/null || uptime)"
echo "---------------------------------------------"
echo "Memory Usage:"
free -h 2>/dev/null || echo "free command not available on this platform"
echo "---------------------------------------------"
echo "Disk Space Usage:"
df -h / | awk 'NR==1 || NR==2'
echo "============================================="
```

---

### 21. Check if a Specific User is Logged In

- **Concepts**: Process inspection, `who`, string filtering with `grep`, exit codes.

```bash
#!/bin/bash
# Description: Checks if a specified user is actively logged into the system

read -p "Enter username to check: " target_user

if who | grep -wq "$target_user"; then
    echo "User '$target_user' is currently LOGGED IN."
    echo "Active sessions:"
    who | grep -w "$target_user"
else
    echo "User '$target_user' is NOT logged in."
fi
```

---

### 22. Find and Delete All Empty (Zero-Byte) Files in a Directory

- **Concepts**: Directory traversal, test operator `-s` (file size > 0), interactive deletion.

```bash
#!/bin/bash
# Description: Searches for 0-byte regular files in a directory and prompts to delete them

read -p "Enter directory path: " dir

if [ ! -d "$dir" ]; then
    echo "Error: Directory '$dir' does not exist."
    exit 1
fi

empty_files=$(find "$dir" -maxdepth 1 -type f -size 0)

if [ -z "$empty_files" ]; then
    echo "No empty files found in '$dir'."
    exit 0
fi

echo "Empty files found:"
echo "$empty_files"

read -p "Do you want to delete these empty files? (y/n): " confirm
if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
    find "$dir" -maxdepth 1 -type f -size 0 -delete
    echo "Empty files deleted successfully."
else
    echo "Deletion cancelled."
fi
```

---

### 23. Compare Two Files and Check if Identical

- **Concepts**: `cmp` utility, standard exit statuses (`0` for match, `1` for difference).

```bash
#!/bin/bash
# Description: Compares two files and checks whether their contents match

if [ $# -ne 2 ]; then
    echo "Usage: $0 <file1> <file2>"
    exit 1
fi

file1="$1"
file2="$2"

if [ ! -f "$file1" ] || [ ! -f "$file2" ]; then
    echo "Error: Both arguments must be valid regular files."
    exit 1
fi

if cmp -s "$file1" "$file2"; then
    echo "Files '$file1' and '$file2' are IDENTICAL."
else
    echo "Files '$file1' and '$file2' are DIFFERENT."
    echo "Differences preview:"
    diff -u "$file1" "$file2" | head -n 15
fi
```

---

### 24. Find Files Modified in the Last N Days or Exceeding a Size

- **Concepts**: Advanced `find` criteria (`-mtime`, `-size`), pipeline formatting.

```bash
#!/bin/bash
# Description: Finds files modified within the last N days or larger than size limit

read -p "Enter directory path: " target_dir
read -p "Find files modified in last how many days? : " days
read -p "Find files larger than size (e.g., +10M, +500k): " size_filter

if [ ! -d "$target_dir" ]; then
    echo "Error: Directory does not exist."
    exit 1
fi

echo "================================================"
echo "Files modified in the last $days days:"
find "$target_dir" -type f -mtime -"$days" -ls 2>/dev/null | head -n 10

echo "------------------------------------------------"
echo "Files larger than $size_filter:"
find "$target_dir" -type f -size "$size_filter" -ls 2>/dev/null | head -n 10
echo "================================================"
```

---

### 25. Automated Timestamped Directory Backup and Compression

- **Concepts**: Date formatting `$(date +%Y%m%d_%H%M%S)`, `tar` archiving with gzip compression (`-czvf`).

```bash
#!/bin/bash
# Description: Creates a compressed, timestamped tar.gz backup of a chosen directory

read -p "Enter directory to backup: " source_dir
read -p "Enter destination backup directory: " backup_dir

if [ ! -d "$source_dir" ]; then
    echo "Error: Source directory '$source_dir' does not exist."
    exit 1
fi

mkdir -p "$backup_dir"

timestamp=$(date +"%Y%m%d_%H%M%S")
archive_name="backup_$(basename "$source_dir")_${timestamp}.tar.gz"
destination_path="${backup_dir}/${archive_name}"

echo "Starting backup of '$source_dir'..."
tar -czf "$destination_path" -C "$(dirname "$source_dir")" "$(basename "$source_dir")"

if [ $? -eq 0 ]; then
    echo "Backup completed successfully!"
    echo "Archive location : $destination_path"
    echo "Archive size     : $(du -h "$destination_path" | cut -f1)"
else
    echo "Backup failed!"
    exit 1
fi
```

---

### 26. Sum of Elements at Odd and Even Indices in an Array

- **Concepts**: Bash arrays, index iteration, modulo arithmetic `$((i % 2))`, 0-based vs 1-based indexing.

```bash
#!/bin/bash
# Description: Computes sum of elements at odd indices and even indices in an array

read -p "Enter numbers separated by spaces: " -a arr
n=${#arr[@]}

sum_even=0
sum_odd=0

for (( i=0; i<n; i++ )); do
    if [ $((i % 2)) -eq 0 ]; then
        sum_even=$((sum_even + arr[i]))
    else
        sum_odd=$((sum_odd + arr[i]))
    fi
done

echo "Array: [${arr[*]}]"
echo "Sum of Elements at EVEN Indices (0, 2, 4, ...): $sum_even"
echo "Sum of Elements at ODD  Indices (1, 3, 5, ...): $sum_odd"
echo "Difference (Even Sum - Odd Sum)               : $((sum_even - sum_odd))"

# Note on 1-based convention:
# If the exam considers the first element as position 1:
# - Position 1, 3, 5 (Odd Positions)   = $sum_even
# - Position 2, 4, 6 (Even Positions)  = $sum_odd
```

**Sample Output:**
```text
Enter numbers separated by spaces: 10 20 30 40 50 60
Array: [10 20 30 40 50 60]
Sum of Elements at EVEN Indices (0, 2, 4, ...): 90
Sum of Elements at ODD  Indices (1, 3, 5, ...): 120
Difference (Even Sum - Odd Sum)               : -30
```

---

### 27. Sum of Even and Odd Digits in a Number

- **Concepts**: Extracting individual digits via `% 10`, conditional parity check on digit value.

```bash
#!/bin/bash
# Description: Calculates sum of even digits and sum of odd digits in a given number

read -p "Enter a number: " num
temp=$num
sum_even_digits=0
sum_odd_digits=0

while [ "$temp" -gt 0 ]; do
    digit=$((temp % 10))
    if [ $((digit % 2)) -eq 0 ]; then
        sum_even_digits=$((sum_even_digits + digit))
    else
        sum_odd_digits=$((sum_odd_digits + digit))
    fi
    temp=$((temp / 10))
done

echo "Number             : $num"
echo "Sum of Even Digits : $sum_even_digits"
echo "Sum of Odd Digits  : $sum_odd_digits"
```

**Sample Output:**
```text
Enter a number: 482591
Number             : 482591
Sum of Even Digits : 14  (4 + 8 + 2)
Sum of Odd Digits  : 15  (5 + 9 + 1)
```

---

### 28. Find the Second Largest Element in an Array

- **Concepts**: Single-pass traversal, maintaining first and second maxima, handling duplicates.

```bash
#!/bin/bash
# Description: Finds the largest and second largest elements in an array

read -p "Enter numbers separated by spaces: " -a arr
n=${#arr[@]}

if [ "$n" -lt 2 ]; then
    echo "Error: Need at least 2 numbers."
    exit 1
fi

largest=-999999
second_largest=-999999

for num in "${arr[@]}"; do
    if [ "$num" -gt "$largest" ]; then
        second_largest=$largest
        largest=$num
    elif [ "$num" -gt "$second_largest" ] && [ "$num" -ne "$largest" ]; then
        second_largest=$num
    fi
done

echo "Array          : ${arr[*]}"
echo "Largest Number : $largest"
echo "Second Largest : $second_largest"
```

---

### 29. Linear Search and Binary Search in an Array

- **Concepts**: Search algorithms, while loop binary division `mid = (low + high) / 2`.

```bash
#!/bin/bash
# Description: Implements Binary Search on a sorted array

read -p "Enter sorted numbers separated by space: " -a arr
read -p "Enter target element to search: " key

low=0
high=$((${#arr[@]} - 1))
found=0

while [ "$low" -le "$high" ]; do
    mid=$(( (low + high) / 2 ))
    
    if [ "${arr[mid]}" -eq "$key" ]; then
        echo "Element $key found at index $mid (position $((mid + 1)))."
        found=1
        break
    elif [ "${arr[mid]}" -lt "$key" ]; then
        low=$((mid + 1))
    else
        high=$((mid - 1))
    fi
done

if [ "$found" -eq 0 ]; then
    echo "Element $key is not present in the array."
fi
```

---

### 30. Count Frequency of Each Element in an Array

- **Concepts**: Associative arrays (`declare -A`), frequency mapping, key iteration.

```bash
#!/bin/bash
# Description: Counts the frequency of occurrence of each element in an array

read -p "Enter elements separated by spaces: " -a arr
declare -A freq

for item in "${arr[@]}"; do
    freq["$item"]=$(( freq["$item"] + 1 ))
done

echo "--- Element Frequency Table ---"
for key in "${!freq[@]}"; do
    echo "Element '$key' appears ${freq[$key]} time(s)"
done
```

---

### 31. Matrix Addition (2D Matrix) in Shell Script

- **Concepts**: Simulating 2D arrays using 1D indexing `[i * cols + j]`, nested loops, formatted printing.

```bash
#!/bin/bash
# Description: Performs addition of two R x C matrices

read -p "Enter number of rows: " rows
read -p "Enter number of cols: " cols

echo "Enter elements of Matrix A ($((rows * cols)) numbers separated by space):"
read -a matA

echo "Enter elements of Matrix B ($((rows * cols)) numbers separated by space):"
read -a matB

echo "--- Resultant Matrix (A + B) ---"
for (( i=0; i<rows; i++ )); do
    for (( j=0; j<cols; j++ )); do
        idx=$(( i * cols + j ))
        sum=$(( matA[idx] + matB[idx] ))
        printf "%4d " "$sum"
    done
    printf "\n"
done
```

---

### 32. Pyramid and Number Triangle Patterns

- **Concepts**: Nested `for` loops, printing spaces and characters, formatting.

```bash
#!/bin/bash
# Description: Prints a star pyramid and Floyd's triangle of height N

read -p "Enter height of pyramid: " n

echo "Star Pyramid:"
for (( i=1; i<=n; i++ )); do
    # Print leading spaces
    for (( s=i; s<n; s++ )); do
        printf " "
    done
    # Print stars
    for (( j=1; j<=(2*i - 1); j++ )); do
        printf "*"
    done
    printf "\n"
done

echo ""
echo "Floyd's Number Triangle:"
count=1
for (( i=1; i<=n; i++ )); do
    for (( j=1; j<=i; j++ )); do
        printf "%-3d " "$count"
        count=$((count + 1))
    done
    printf "\n"
done
```

---

### 33. Count Vowels, Consonants, Digits, and Spaces in a String

- **Concepts**: String character iteration `${str:i:1}`, regex pattern matching `[[ $char =~ ... ]]`.

```bash
#!/bin/bash
# Description: Counts vowels, consonants, digits, spaces, and special symbols in a string

read -p "Enter a string: " input

vowels=0
consonants=0
digits=0
spaces=0
special=0

len=${#input}

for (( i=0; i<len; i++ )); do
    ch="${input:i:1}"
    case "$ch" in
        [aeiouAEIOU]) vowels=$((vowels + 1)) ;;
        [a-zA-Z])    consonants=$((consonants + 1)) ;;
        [0-9])        digits=$((digits + 1)) ;;
        [[:space:]])  spaces=$((spaces + 1)) ;;
        *)            special=$((special + 1)) ;;
    esac
done

echo "String Analysis for: \"$input\""
echo "Vowels             : $vowels"
echo "Consonants         : $consonants"
echo "Digits             : $digits"
echo "Spaces             : $spaces"
echo "Special Characters : $special"
echo "Total Length       : $len"
```

---

### 34. Check if Two Strings are Anagrams

- **Concepts**: String sorting using `fold`, `sort`, `paste`, equality check.

```bash
#!/bin/bash
# Description: Checks whether two words are anagrams of each other

read -p "Enter first string : " str1
read -p "Enter second string: " str2

# Clean strings (lowercase and remove non-letters)
s1=$(echo "$str1" | tr '[:upper:]' '[:lower:]' | tr -cd '[:alpha:]' | fold -w1 | sort | tr -d '\n')
s2=$(echo "$str2" | tr '[:upper:]' '[:lower:]' | tr -cd '[:alpha:]' | fold -w1 | sort | tr -d '\n')

if [ "$s1" = "$s2" ]; then
    echo "\"$str1\" and \"$str2\" are ANAGRAMS."
else
    echo "\"$str1\" and \"$str2\" are NOT anagrams."
fi
```

---

### 35. Decimal to Binary and Binary to Decimal Conversion

- **Concepts**: Number base conversion, binary weights, `bc` base translation `obase=2; ibase=2`.

```bash
#!/bin/bash
# Description: Base conversion between Decimal and Binary

echo "1. Decimal to Binary"
echo "2. Binary to Decimal"
read -p "Choose option [1 or 2]: " choice

case $choice in
    1)
        read -p "Enter decimal number: " dec
        bin=$(echo "obase=2; $dec" | bc)
        echo "Binary equivalent: $bin"
        ;;
    2)
        read -p "Enter binary string: " bin
        dec=$(echo "ibase=2; $bin" | bc)
        echo "Decimal equivalent: $dec"
        ;;
    *)
        echo "Invalid choice."
        ;;
esac
```

---

## 💡 Quick Tips for Lab Exams & Viva

1. **Shebang Line**: Always start shell scripts with `#!/bin/bash` or `#!/bin/sh`.
2. **Spacing in Brackets**: In `[ condition ]`, spaces after `[` and before `]` are **mandatory** because `[` is a command.
3. **Variable Quoting**: Always quote your variables (e.g., `"$var"`) to handle spaces and empty values gracefully.
4. **Exit Codes**: `$?` holds the return status of the last executed command (`0` = success, non-zero = failure).
5. **Arithmetic Evaluation**: Use `$(( expression ))` for integer math; use `bc` for floating-point math (e.g., `echo "scale=2; 5/2" | bc`).
