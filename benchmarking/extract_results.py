import glob
import csv
from argparse import ArgumentParser

class Solution:
    def __init__(self, instance_name: str, search: str, branching: str, objective_values: list,
                 time_stamps: list, solutions_by_order: list, solutions_by_item: list):
        self._instance_name = instance_name
        self._search = search
        self._branching = branching
        self._objective_values = objective_values
        self._time_stamps = time_stamps
        self._solutions_by_order = solutions_by_order
        self._solutions_by_item = solutions_by_item

    def print(self):
        print("objective = " + str(self._objective_values[len(self._objective_values) - 1]) + "\n")
        print("time = " + str(self._objective_values[len(self._time_stamps) - 1]) + " ms\n")
        print("solution_by_orders = " + self._solutions_by_order[len(self._solutions_by_order) - 1] + " ms\n")

    def write_into_csv_file(self):
        filename = self._instance_name + "_" + str(self._search) + "_" + str(self._branching) + ".csv"

        with open(filename, 'w') as f:
            fieldnames = ['objective', 'time_ms', 'production_orders', 'production_items']
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            for idx, obj in enumerate(self._objective_values):
                writer.writerow({'objective': self._objective_values[idx], 'time_ms': self._time_stamps[idx],
                                 'production_orders': self._solutions_by_order[idx],
                                 'production_items': self._solutions_by_item[idx]})
            f.close()

    def has_solution_with_index(self, index: int) -> bool:
        return len(self._time_stamps) > index


def parse_args():
    parser = ArgumentParser(description="extracting solution")
    parser.add_argument("-f", dest="folder", required=True,
                        help="path to folder", type=str)
    return parser.parse_args()


def read_solution_file(input_filename: str) -> Solution:
    file = open(input_filename, "r")

    # read info from the file name: <instance_name>.txt_<search>_<branching>.sol
    input = input_filename.split(".")
    instance_name = input[0]
    print("instance_name = " + instance_name)
    input_parts = input[1].split("_")
    search_str = input_parts[1]
    print("search = " + search_str)
    branching_type = input_parts[2].split(".")[0]
    print("branching = " + branching_type)

    # collect the info line by line
    solutions_by_order = []
    solutions_by_item = []
    time = []
    objective = []
    lines = file.readlines()
    for line in lines:
        print("processing line: " + line)
        if line.startswith("--"):
            continue
        elif line.startswith("production_by_order"):
            solutions_by_order.append(line.split("=")[1].split(";")[0])
        elif line.startswith("production_by_item"):
            solutions_by_item.append(line.split("=")[1].split(";")[0])
        elif line.startswith("objective"):
            objective.append(int(line.split("=")[1].split(";")[0]))
        elif line.startswith("time"):
            time.append(int(line.split("=")[1].split("ms")[0]))
        else:
            continue
    return Solution(instance_name=instance_name, search=search_str, branching=branching_type,
                    solutions_by_order=solutions_by_order, solutions_by_item=solutions_by_item, time_stamps=time,
                    objective_values=objective)


def main():
    args = parse_args()
    solution_files = glob.glob(args.folder + "/*.sol")
    for solution_file in solution_files:
        print("Reading solution " + solution_file)
        solution = read_solution_file(solution_file)
        solution.print()
        solution.write_into_csv_file()


if __name__ == "__main__":
    main()
