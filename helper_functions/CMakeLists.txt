add_executable(boundaryprettyfy boundaryprettyfy.cpp)


add_executable(stringify_values stringify_value_json.cpp)
target_link_libraries(stringify_values PRIVATE misc)

add_executable(get_mean_sigma get_mean_sigma.cpp)

add_executable(get_quantiles get_quantiles.cpp)
add_executable(compute_reference_discrepance compute_reference_discrepance.cpp)

add_executable(oup oup.cpp)
target_link_libraries(oup stochastics)


add_executable(generate_printing_csv generate_printing_csv.cpp)
target_link_libraries(generate_printing_csv PRIVATE input_output)

add_executable(generate_control_printing_csv generate_control_printing_csv.cpp)
target_link_libraries(generate_control_printing_csv PRIVATE input_output)


add_executable(move_to_category move_to_category.cpp)
add_executable(normalize_jsons normalize_jsons.cpp)
add_executable(draw_gaslib_graph draw_gaslib_graph.cpp)



add_executable(extract_new_initial_condition extract_new_initial_condition.cpp)


add_executable(pv_to_vphi pv_to_vphi.cpp)
add_executable(construct_gaspowerconnection construct_gaspowerconnection.cpp)



add_executable(sum_outflow sum_outflow.cpp)
add_executable(change_inflow change_inflow.cpp)
add_executable(increase_pressure increase_pressure.cpp)
add_executable(include_seed_in_boundaryvalues include_seed_in_boundaryvalues.cpp)
add_executable(remove_seed_from_boundaryvalues remove_seed_from_boundaryvalues.cpp)

add_executable(draw_heatmap_gaslib draw_heatmap_gaslib.cpp)
target_link_libraries(draw_heatmap_gaslib PUBLIC netfactory componentjsonhelpers)

add_executable(draw_heatmap_power draw_heatmap_power.cpp)
target_link_libraries(draw_heatmap_power PUBLIC netfactory componentjsonhelpers)

add_executable(draw_power_graph draw_power_graph.cpp)




add_executable(insert_positions insert_positions.cpp)
target_link_libraries(insert_positions PUBLIC exception)
