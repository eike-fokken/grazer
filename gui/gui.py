from gooey import Gooey, GooeyParser


@Gooey(target="grazer run", program_name="Grazer", suppress_gooey_flag=True)
def main():
    parser = GooeyParser(description="Simulation of dynamical systems")
    # grazer_run = parser.add_argument_group("run")
    parser.add_argument(
        "grazer-directory",
        metavar="Grazer Directory",
        widget="DirChooser",
        help="Directory containing a 'problem' folder, will contain an output folder after grazer is run",
    )

    # schema = parser.add_argument_group("schema")
    parser.parse_args()

if __name__ == "__main__":
    main()