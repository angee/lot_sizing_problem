import csv
from enum import Enum


# class Branching(Enum):
#     """
#     The different branching options for the CP implementation in Gecode
#     """
#     SDF_RANDOM = 1
#     SDF_GREEDY = 2
#     STATIC_GREEDY = 3
#     HYBRID_GREEDY = 4
#
#
# class Search(Enum):
#     """
#     The search variants for the CP implemenation in Gecode
#     """
#     EXACT = 1
#     LNS = 2


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
        print("objective = " + self._objective_values[len(self._objective_values) - 1] + "\n")
        print("time = " + self._objective_values[len(self._time_stamps) - 1] + " ms\n")
        print("solution_by_orders = " + self._solutions_by_order[len(self._solutions_by_order) - 1] + " ms\n")

    def write_into_csv_file(self):
        filename = self._instance_name + "_" + str(self._search) + "_" + str(self._branching) + ".csv"

        with open(filename, 'w') as f:
            fieldnames = ['objective', 'time_ms', 'production_orders', 'production_items']
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            for idx, obj in self._objective_values:
                writer.writerow({'objective': self._objective_values[idx], 'time_ms': self._time_stamps[idx],
                                 'production_orders': self._solutions_by_order[idx],
                                 'production_items': self._solutions_by_item[idx]})
            f.close()

    def haSolutionWithIndex(self, index: int) -> bool:
        return len(self._time_stamps) > index
