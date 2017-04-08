
import os

def delete_file_folder(src):  
    '''delete files and folders''' 
    if os.path.isfile(src):  
        try:  
            os.remove(src)  
        except:  
            pass 
    elif os.path.isdir(src):  
        for item in os.listdir(src):  
            itemsrc=os.path.join(src,item)  
            delete_file_folder(itemsrc)  
        try:  
            os.rmdir(src)  
        except:  
            pass 


def exe_command(command, no_command = False):
	command_exe = 'sudo ../build/demo '+ command
	print command_exe
	if not no_command:
		os.system(command_exe)


def make_dir(dir_name):
    command = 'sudo -u long mkdir -p ' + dir_name
    changepermission = 'chmod 666 ' + dir_name
    os.system(command)
    #os.system(changepermission)

def make_dirs(dir_name):
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)