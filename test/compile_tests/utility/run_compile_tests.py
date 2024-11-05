import subprocess
from tempfile import TemporaryDirectory


class Compiler:
    def __init__(self, compiler, *global_args, verbose=False):
        self.compiler = compiler
        self.global_args = global_args
        self.verbose = verbose
        self.temp_out_dir = TemporaryDirectory()

    def cleanup(self):
        self.temp_out_dir.cleanup()

    def compiles(self, filename, *args):
        cmd = [self.compiler, '-o', f'{self.temp_out_dir.name}/a.out', *self.global_args, *args, filename]
        try:
            result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            if self.verbose:
                print(f"Compilation successful: {' '.join(cmd)}")
                print(result.stdout.decode('utf-8'))
            return True
        except subprocess.CalledProcessError as err:
            if self.verbose:
                print(f"Compilation failed: {' '.join(cmd)}")
                print(err.stdout.decode('utf-8'))
            return False

    def compile_fails(self, filename, *args):
        return not self.compiles(filename, *args)
