import csv
import glob

from argparse import ArgumentParser

from solution_processing.solution import Solution
from solution_processing.solution_reader import SolutionReader


def parse_args():
    parser = ArgumentParser(description="extracting solution")
    parser.add_argument("-i", dest="folder", required=True,
                        help="path to folder", type=str)
    parser.add_argument("-o", dest="output_filename", required=False, help="filename for CSV output", type=str)
    return parser.parse_args()


def main():
    args = parse_args()
    solution_files = glob.glob(args.folder + "/*.sol")
    # write solutions into one CSV file
    csv_filename = args.output_filename if not args.output_filename is None else "solutions.csv"
    with open(csv_filename, 'w') as f:
        writer = csv.DictWriter(f, fieldnames=Solution.get_csv_header())
        writer.writeheader()
        for solution_file in solution_files:
            print("Reading solution " + solution_file)
            reader = SolutionReader(solution_file)
            solution = reader.create_solution()
            solution.write_into_detailed_csv_file()
            writer.writerow(solution.get_csv_row())
        f.close()


if __name__ == "__main__":
    main()
