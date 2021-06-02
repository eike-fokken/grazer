add_executable(boundaryprettyfy boundaryprettyfy.cpp)

add_executable(transform_boundary transform_boundary.cpp)

add_executable(stringify_values stringify_value_json.cpp)
target_link_libraries(stringify_values PRIVATE misc)

add_executable(transform_initial transform_initial.cpp)

add_executable(get_mean_sigma get_mean_sigma.cpp)

add_executable(generate_printing_csv generate_printing_csv.cpp)
target_link_libraries(generate_printing_csv PRIVATE input_output)

add_executable(move_to_category move_to_category.cpp)
add_executable(normalize_jsons normalize_jsons.cpp)
add_executable(draw_gaslib_graph draw_gaslib_graph.cpp)

add_executable(extract_new_initial_condition extract_new_initial_condition.cpp)


add_executable(pv_to_vphi pv_to_vphi.cpp)
add_executable(construct_gaspowerconnection construct_gaspowerconnection.cpp)



#temporary:
add_executable(sum_outflow sum_outflow.cpp)
