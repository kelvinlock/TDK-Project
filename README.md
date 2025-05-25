# TDK-Project
*Created by [Kelvin](https://github.com/kelvinlock/TDK-Project) - November 2023*
### **Summary of Taiwan TDK Cup National University Robotics Competition**  

The **Taiwan TDK Cup National University Creative Design and Robotics Competition** is one of Taiwan’s longest-running and largest robotics contests, co-organized by the Ministry of Education and the TDK Foundation since 1997.  

- **Objective**: To cultivate students' innovation, design, and hands-on integration skills in robotics.  
- **Competition Categories**:  
  - **Remote Control Group**: Manual operation tasks (e.g., obstacle crossing, item transport).  
  - **Autonomous Group**: AI-driven robots with vision recognition and path planning.  
  - **Flying Group** (occasional): UAV challenges like tunnel navigation and precision drops.  
- **Themes & Challenges**: Tasks are inspired by local culture or global issues (e.g., environmental protection, regional-themed missions like "Yunlin Hero Challenge" in 2025).  
- **Prizes**: Winners receive up to **NT$200,000**, with opportunities for international exchanges (e.g., visits to Japan’s NHK Robocon).  
- **Educational Impact**: Emphasizes practical skills in automation, AI, and mechanical design, aligning with industry needs.  
- **Recent Updates**: Hybrid/online formats were adopted during the pandemic, and some categories (e.g., Flying Group) may vary by year.  
For details, visit the [official website](https://web02.yuntech.edu.tw/~tdk_4hhoerjt/) or follow "Taiwan TDK Robocon" on Facebook.

## Package Requirements

* pygame
* pyserial

```bash
pip install pygame
pip install pyserial
```

## Problem might overcome
```bash
module 'serial' has no attribute 'Serial'
```
The error `module 'serial' has no attribute 'Serial'` usually means **Python is importing the wrong module** — often a local file named `serial.py` that shadows the `pyserial` package.

### Here's how to fix it:

#### ✅ Step 1: Check for a conflicting file

Make sure you **don’t have a file named `serial.py`** (or a folder named `serial/`) in your project directory.

* If yes, **rename it** to something else, like `serial_test.py`.
* Also, delete any corresponding `serial.pyc` or `__pycache__/` folders that might still contain cached versions.

#### ✅ Step 2: Test your import

Open a Python shell and run:

```python
import serial
print(serial.__file__)
```

If it prints a path to your own file (e.g., `.../serial.py`), you're still importing the wrong module.

It should point to something like:

```
.../site-packages/serial/__init__.py
```

#### ✅ Step 3: Confirm installation

If you're unsure whether pyserial is installed correctly, you can reinstall it:

```bash
pip uninstall serial
pip uninstall pyserial
pip install pyserial
```

Then test again with:

```python
import serial
print(serial.Serial)
```
