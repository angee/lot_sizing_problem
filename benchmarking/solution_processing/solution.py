import csv


class Solution:
    def __init__(self, instance_name: str, search: str, branching: str, objective_values: list,
                 time_stamps: list, solutions_by_order: list, solutions_by_item: list, nodes: int,
                 failures: int, solutions: int, runtime_sec: float, num_timeslots: int, num_item_types: int):
        self._instance_name = instance_name
        self._search = search
        self._branching = branching
        self._objective_values = objective_values
        self._time_stamps_ms = time_stamps
        self._solutions_by_order = solutions_by_order
        self._solutions_by_item = solutions_by_item
        self._nodes = nodes
        self._failures = failures
        self._solutions = solutions
        self._runtime_secs = runtime_sec
        self._num_timeslots = num_timeslots
        self._num_item_types = num_item_types

    def print(self):
        print("objective = " + str(self._objective_values[len(self._objective_values) - 1]) + "\n")
        print("time = " + str(self._objective_values[len(self._time_stamps_ms) - 1]) + " ms\n")
        print("solution_by_orders = " + self._solutions_by_order[len(self._solutions_by_order) - 1] + " ms\n")

    def write_into_detailed_csv_file(self):
        filename = self._instance_name + "_" + str(self._search) + "_" + str(self._branching) + ".csv"

        with open(filename, 'w') as f:
            fieldnames = ['objective', 'time_ms', 'production_orders', 'production_items']
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            for idx, obj in enumerate(self._objective_values):
                writer.writerow({'objective': self._objective_values[idx], 'time_ms': self._time_stamps_ms[idx],
                                 'production_orders': self._solutions_by_order[idx],
                                 'production_items': self._solutions_by_item[idx]})
            f.close()

    def get_csv_row(self) -> dict:
        return {'objective': self._objective_values[-1] if len(self._objective_values) > 0 else None,
                'runtime_sec': self._runtime_secs,
                'solutions': self._solutions, 'failures': self._failures, 'branching': self._branching,
                'nodes': self._nodes, 'instance_name': self._instance_name, 'num_timeslots': self._num_timeslots,
                'num_item_types': self._num_item_types}

    @staticmethod
    def get_csv_header() -> list:
        return ['objective', 'runtime_sec', 'solutions', 'failures', 'branching', 'nodes', 'num_timeslots',
                'num_item_types', 'instance_name', ]

    def has_solution_with_index(self, index: int) -> bool:
        return len(self._time_stamps_ms) > index
