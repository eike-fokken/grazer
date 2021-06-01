function dir_list = get_all_output_directories(working_directory,output_directory_trunk)

  dir_list=dir(working_directory);
  index=1;
  while(index~=length(dir_list)+1)
    if(strfind(dir_list(index).name,output_directory_trunk))
      index=index+1;
    else
      dir_list(index)=[];
    end
  end
  

end
