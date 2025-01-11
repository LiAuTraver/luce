include_guard()

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# sort targets into corresponding folders
function(add_folder FOLDER_NAME)
  get_property(targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)

  foreach(target IN LISTS targets)
    get_property(folder TARGET "${target}" PROPERTY FOLDER)

    if(NOT DEFINED folder OR folder STREQUAL "")
      set(folder Utility)
      get_property(target_type TARGET "${target}" PROPERTY TYPE)

      if(NOT target_type STREQUAL "UTILITY")
        # not utils, create a new folder for it
        set(folder "${FOLDER_NAME}")
      endif()

      # a utility, put it in the Utility folder
      # note: CMake does not allow two targets with the same name globally,
      # it's safe to do so
      set_property(TARGET "${target}" PROPERTY FOLDER "${folder}")
    endif()
  endforeach()
endfunction()