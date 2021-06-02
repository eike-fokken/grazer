import sys
from pathlib import Path
from gooey import Gooey, GooeyParser


grazer_executable = "grazer"
if sys.platform.startswith("win"):
    grazer_executable += ".exe"
parent_dir = Path(__file__).parent
grazer_path = parent_dir.joinpath("bin/"+grazer_executable).absolute()

@Gooey(target=str(grazer_path), program_name="Grazer", suppress_gooey_flag=True)
def main():
    # make sure that the output is not buffered (cf. https://chriskiehl.com/article/packaging-gooey-with-pyinstaller)
    # nonbuffered_stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
    # sys.stdout = nonbuffered_stdout
    #### 


    parser = GooeyParser(description="Simulation of dynamical systems")
    grazer_subparser = parser.add_subparsers(help="Programs")
    grazer_run = grazer_subparser.add_parser("run", help="Run Simulation")
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