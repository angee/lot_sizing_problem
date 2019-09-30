from solution_processing.solution import Solution


class SolutionReader:
    """
    Given a solution input file, the solution reader extracts the content of the solution and statistics file and
    stores the information in a Solution object, on request
    """

    def __init__(self, solution_filename: str):
        self._base_filename = solution_filename.split(".s")[0]
        self._instance_name = solution_filename.split(".")[0]
        self._num_timeslots = 0
        self._num_item_types = 0
        self.__read_information_from_instance_name()
        self._solutions_by_order = []
        self._solutions_by_item = []
        self._time_ms = []
        self._objective = []
        self._runtime_sec = 0.0  # in seconds
        self._nodes = 0
        self._failures = 0
        self._solutions = 0
        self.__read_solution_file()
        self.__read_stats_file()

    def __read_information_from_instance_name(self):
        """
        Read info from the file name: <instance_name>.txt_<search>_<branching>.sol
        :return: void
        """
        filename = self._base_filename
        input_parts = filename.split(".")[1].split("_")
        self._search = input_parts[1]
        self._branching_type = input_parts[2].split(".")[0]
        instance_name = self._instance_name
        # <name-instance>_Xtimeslots_Ytypes.<ending>
        input_parts = instance_name.split("_")
        self._num_timeslots = int(input_parts[1].split("t")[0])
        self._num_item_types = int(input_parts[2].split("t")[0])

    def __read_solution_file(self):
        file = open(self._base_filename + ".sol", "r")
        lines = file.readlines()
        for line in lines:
            if line.startswith("--"):
                continue
            elif line.startswith("production_by_order"):
                self._solutions_by_order.append(line.split("=")[1].split(";")[0])
            elif line.startswith("production_by_item"):
                self._solutions_by_item.append(line.split("=")[1].split(";")[0])
            elif line.startswith("objective"):
                self._objective.append(int(line.split("=")[1].split(";")[0]))
            elif line.startswith("time"):
                self._time_ms.append(int(line.split("=")[1].split("ms")[0]))
            else:
                continue

    def __read_stats_file(self):
        file = open(self._base_filename + ".stats", "r")
        lines = file.readlines()
        for line in lines:
            if line.startswith("\truntime"):
                self._runtime_sec = float(line.split("e:")[1].split("(")[1].split("m")[0]) / 1000
            elif line.startswith("\tnodes"):
                self._nodes = int(line.split(":")[1].strip())
            elif line.startswith("\tfail"):
                self._failures = int(line.split(":")[1].strip())
            elif line.startswith("\tsolu"):
                self._solutions = int(line.split(":")[1].strip())
            else:
                continue

    def create_solution(self) -> Solution:
        return Solution(instance_name=self._instance_name, search=self._search, branching=self._branching_type,
                        solutions_by_order=self._solutions_by_order, solutions_by_item=self._solutions_by_item,
                        time_stamps=self._time_ms, objective_values=self._objective, solutions=self._solutions,
                        nodes=self._nodes, failures=self._failures, runtime_sec=self._runtime_sec,
                        num_timeslots=self._num_timeslots, num_item_types=self._num_item_types)
