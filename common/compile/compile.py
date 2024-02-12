from os.path import exists
from os import makedirs, remove, removedirs, getcwd, chdir
from datetime import datetime
from shutil import which
from re import sub
from subprocess import call
from platform import system


PROJECT_NAME: str = 'Sen.Environment'

PROJECT_RESPIRATORY: str = 'https://github.com/Haruma-VN/Sen.Environment'

class Timer:

	# Constructor

	def __init__(self) -> None:
		self.duration = None
		self.time = None
		return

	# Start

	def start(self) -> None: 
		if self.time is not None:
			return
		self.time = datetime.now()
		return

	# End
	
	def end(self) -> None:
		duration: float = datetime.now() - self.time
		if self.duration is None:
			self.duration = duration
			return
		self.duration += duration
		return

class FileSystem:

	@staticmethod
	def exists(destination: str) -> bool:
		return exists(destination)

	@staticmethod
	def make_directory(destination: str) -> None:
		if not FileSystem.exists(destination): 
			makedirs(destination) 
		return 

	@staticmethod
	def write_file(destination: str, content: str) -> None:
		with open(destination, 'w') as f:
			f.write(content)
		return

	@staticmethod
	def delete_file(destination: str) -> None:
		remove(destination)
		return
	
	@staticmethod
	def delete_directory(destination: str) -> None:
		removedirs(destination)
		return
	
	@staticmethod
	def current_workspace() -> str:
		return getcwd() 

class System:
	@staticmethod
	def exists(cmd: str) -> bool:
		return which(cmd) is not None
	
	@staticmethod
	def make_posix_path(path: str) -> str:
		return sub(r'\\', '/', path)
	
	@staticmethod
	def change_workspace(destination: str) -> None:
		chdir(destination)
		return
	
	@staticmethod
	def current() -> str: 
		return system()
	
class Process:
	@staticmethod
	def spawn(variable: str) -> None:
		call(variable, shell=True)
		return
	
class Build:
	@staticmethod
	def kernel(source: str) -> None:
		if System.current() == 'Windows':
			System.change_workspace(source)
			#Process.spawn(f'powershell -noexit "& "{source}/build.ps1"')
		else:
			System.change_workspace(source)
			Process.spawn(f'chmod +x ./build.sh')
			Process.spawn(f'./build.sh')
		return
	
	@staticmethod
	def script(source: str) -> None:
		if System.current() == 'Windows':
			System.change_workspace(source)
			Process.spawn(f'powershell -noexit "& "{source}/link.ps1"')
		else:
			# need to add the support here
			raise Exception('unsupported')
		return
	
	@staticmethod
	def shell(source: str) -> None:
		if System.current() == 'Windows':
			System.change_workspace(source)
			Process.spawn(f'powershell -noexit "&" "{source}/build.ps1"')
		else:
			raise Exception('unsupported')
		return

PROJECT_BUILD: dict = {
	'Shell': Build.shell, 
	'Script': Build.script, 
	'Kernel': Build.kernel,
}

if __name__ == "__main__":
	if not System.exists('git'):
		raise Exception('git is not found in path environment')
	clock: Timer = Timer()
	clock.start()
	current_directory: str = System.make_posix_path(FileSystem.current_workspace())
	compile_destination: str = f'{current_directory}/{PROJECT_NAME}'
	FileSystem.make_directory(compile_destination)
	print('prepare done')
	Process.spawn(f'cd {compile_destination}')
	print('cloning project from github')
	Process.spawn(f'git clone {PROJECT_RESPIRATORY}')
	for project, callback in PROJECT_BUILD.items():
		callback(f'{compile_destination}/{project}')
	clock.end()
	print(f'Time spent: {clock.duration}')
