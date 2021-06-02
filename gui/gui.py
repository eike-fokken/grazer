from gooey import Gooey, GooeyParser


@Gooey(target="grazer", program_name="Grazer", suppress_gooey_flag=True)
def main():
    parser = GooeyParser(description="Simulation of dynamical systems")
    grazer_subparser = parser.add_subparsers(help="Programs")
    grazer_run = grazer_subparser.add_parser("run")
    grazer_run_group = grazer_run.add_argument_group("Run Simulation")
    grazer_run_group.add_argument(
        "grazer-directory",
        metavar="Grazer Directory",
        widget="DirChooser",
        help="Directory containing a 'problem' folder, will contain an output folder afterwards",
    )

    make_full_factory = grazer_subparser.add_parser("schema make-full-factory")
    make_full_factory.add_argument(
        "grazer-directory",
        metavar="Grazer Directory",
        widget="DirChooser",
        help="Directory containing a 'problem' folder, will contain a 'schemas' folder afterwards",
    )
    insert_key = grazer_subparser.add_parser("schema insert-key")
    insert_key.add_argument(
        "grazer-directory",
        metavar="Grazer Directory",
        widget="DirChooser",
        help="Directory containing a 'problem' and 'schema' folder, modifies the problem files with the schema key",
    )
    parser.parse_args()

if __name__ == "__main__":
    main()