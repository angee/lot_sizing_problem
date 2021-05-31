# Transforms the .txt benchmark files into
#  1. MiniZinc's data format .dzn for the MiniZinc models (CP and MIP)
#  2. Matlab's data format .m for the Matlab model (SDP)
import os
from argparse import ArgumentParser


def is_valid_file(parser, arg):
    """
    check if the given input file exists
    :param parser:
    :param arg:
    :return:
    """
    if not os.path.exists(arg):
        parser.error("The file %s does not exist!" % arg)
    else:
        return arg


def parse_args():
    """
    Parsing the command line arguments
    :return:
    """
    parser = ArgumentParser(description="Lot sizing instance file transformer")
    parser.add_argument("-i", dest="filename", required=True,
                        help="benchmark txt file", metavar="FILE",
                        type=lambda x: is_valid_file(parser, x))
    parser.add_argument("-t", dest="output_type", required=True, help="The output instance format. Choose from "
                                                                      "minizinc and matlab.")
    return parser.parse_args()


def extract_info_from_file(filename: str) -> (int, int, list, list, int, int, list):
    with open(filename) as f:
        lines = f.readlines()
        nb_periods = int(lines[0])
        nb_item_types = int(lines[1])
        # read the due dates and orders
        orders_per_item_type, due_period, nb_orders = extract_orders_and_due_time(lines=lines,
                                                                                  nb_item_types=nb_item_types)
        # read the inventory cost
        print("inventory_cost = " + str(lines[2 + nb_item_types]))
        inventory_cost = int(lines[2 + nb_item_types])
        # read the change cost matrix
        change_cost = extract_change_cost(lines=lines, nb_item_types=nb_item_types)
    return nb_periods, nb_item_types, orders_per_item_type, due_period, nb_orders, inventory_cost, change_cost


def extract_orders_and_due_time(lines: list, nb_item_types: int) -> (list, list, int):
    orders_per_item_type = []
    due_period = []
    for item in range(nb_item_types):
        line = lines[item + 2].split(' ')
        nb_orders_per_item_type = 0
        for idx, val in enumerate(line):
            if val == "1" or val == "1\n":
                nb_orders_per_item_type += 1
                due_period.append(idx)
        orders_per_item_type.append(nb_orders_per_item_type)
    nb_orders = sum(orders_per_item_type)
    return orders_per_item_type, due_period, nb_orders


def extract_change_cost(lines: list, nb_item_types: int) -> list:
    change_cost = [[] for _ in range(nb_item_types)]
    for item1 in range(nb_item_types):
        line = lines[4 + nb_item_types + item1].split(' ')
        for item2 in range(nb_item_types):
            change_cost[item1].append(int(line[item2]))
    return change_cost


def generate_dzn_file(filename: str):
    nb_periods, nb_item_types, orders_per_item_type, due_period, nb_orders, inventory_cost, change_cost = extract_info_from_file(
        filename)
    with open(filename + ".dzn", "w") as f:
        f.write("% Automatically generated DZN file for Lot Sizing Problem\n\n")
        f.write("nb_item_types = " + str(nb_item_types) + ";\n")
        f.write("nb_orders = " + str(nb_orders) + ";\n")
        f.write("nb_periods = " + str(nb_periods) + ";\n")
        f.write("inventory_cost = " + str(inventory_cost) + ";\n\n")
        # map orders to item types
        order_cnt = 1
        item_type_of_order = []
        for idx, orders in enumerate(orders_per_item_type):
            f.write("% item-type-" + str(idx + 1) + ":\t\t")
            for order in range(orders):
                f.write("order-" + str(order_cnt) + ", ")
                order_cnt += 1
                item_type_of_order.append(idx + 1)
            f.write("\n")
        # write due period
        f.write("\ndue_period = [ ")
        for due_time in due_period:
            f.write(str(due_time + 1) + ", ")
        f.write(" ];\n")
        # write change costs
        f.write("change_cost = array2d(Orders0, Orders0, [\n% \t")
        for order in range(nb_orders + 1):
            if order == 0:
                f.write("0   ")
            else:
                f.write("o" + str(order) + "  ")
        f.write("\n\t ")
        # it's zero cost from no production to an item type
        for order in range(nb_orders + 1):
            f.write("0,  ")
        f.write(" % 0")
        for o1 in range(nb_orders):
            f.write("\n\t 0,  ")
            for o2 in range(nb_orders):
                item1 = item_type_of_order[o1] - 1
                item2 = item_type_of_order[o2] - 1
                f.write(str(change_cost[item1][item2]) + ",  ")
            f.write(" % o" + str(o1 + 1))
        f.write("\n]);\n")
        # write nb_of_orders
        f.write("\nnb_of_orders = [")
        for nb in orders_per_item_type:
            f.write(str(nb) + ", ")
        f.write("];\n")
        f.write("item_type = array1d(Orders0, [0, ")
        for i_type in item_type_of_order:
            f.write(str(i_type) + ", ")
        f.write("]);\n")
    f.close()
    print("Written DZN file into: " + filename + ".dzn")


def generate_matlab_file(filename: str):
    nb_periods, nb_item_types, orders_per_item_type, due_period, nb_orders, inventory_cost, change_cost = extract_info_from_file(
        filename)
    # due period is per order
    # orders per item type:
    m = nb_periods
    n = nb_item_types
    with open(filename + ".m", "w") as f:
        f.write("% Automatically generated Matlab file for Lot Sizing Problem\n%\n")
        f.write("% Instance size: " + str(nb_periods) + " periods, " + str(nb_item_types) + " item types\n%\n")
        f.write(
            "% h - storage cost\n% C - {0}x{1} matrix containing change-over costs between {2} different item types\n".format(
                str(n), str(n), str(n)))
        f.write("% D - " + str(m) + "x" + str(n) + " matrix containing demand D(p,t) for period p in 1.." + str(m) +
                " and item type t in 1.." + str(n) + "\n")
        f.write("h = " + str(inventory_cost) + ";\n\n")
        # write change-over costs
        f.write("C = [\n")
        for type1 in range(nb_item_types):
            for type2 in range(nb_item_types):
                f.write(" " + str(change_cost[type1][type2]) + " ")
            f.write(";\n")
        f.write("];\n")
        # ---- write demand ------
        # map orders to item types
        order_cnt = 1
        item_type_of_order = []
        for idx, orders in enumerate(orders_per_item_type):
            # f.write("% item-type-" + str(idx + 1) + ":\t\t")
            for order in range(orders):
                # f.write("order-" + str(order_cnt) + ", ")
                order_cnt += 1
                item_type_of_order.append(idx + 1)
            # f.write("\n")
        # initialize demand array with zeros
        demand = []
        for p in range(nb_periods):
            demand.append([0]*nb_item_types)
        for order in range(nb_orders):
            item_type = item_type_of_order[order]
            period = due_period[order]
            demand[period-1][item_type-1] = 1
        f.write("D = [\n");
        for p in range(nb_periods):
            for t in range(nb_item_types):
                f.write(" " + str(demand[p][t]) + " ")
            f.write(";\n")
        f.write("];\n")
    f.close()
    print("Written Matlab instance file into: " + filename + ".m")


def main():
    args = parse_args()
    if args.output_type == "minizinc":
        generate_dzn_file(args.filename)
    elif args.output_type == "matlab":
        generate_matlab_file(args.filename)
    else:
        print("Unknown output type:" + args.output_type + ", expected minizinc or matlab.")


if __name__ == "__main__":
    main()
