import subprocess


def check_compiler_exists(compiler="g++"):
    try:
        subprocess.check_call([compiler, "--version"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print(f"Compiler '{compiler}' not found. Please ensure it is installed and in your PATH.")
        return False
