one_inst_cnt = dict()
two_inst_cnt = dict()
three_inst_cnt = dict()

with open('highway.txt') as file:
    lines = [line.rstrip() for line in file]

for i in range(len(lines) - 2):
    one_inst_cnt[lines[i]] = one_inst_cnt.get(lines[i], 0) + 1
    two_inst = lines[i] + "_" + lines[i + 1]
    two_inst_cnt[two_inst] = two_inst_cnt.get(two_inst, 0) + 1
    three_inst = lines[i] + "_" + lines[i + 1] + "_" + lines[i + 2]
    three_inst_cnt[three_inst] = three_inst_cnt.get(three_inst, 0) + 1

one_inst_result = []
two_inst_result = []
three_inst_result = []
for key in one_inst_cnt.keys():
    one_inst_result.append([key, one_inst_cnt[key]])
for key in two_inst_cnt.keys():
    two_inst_result.append([key, two_inst_cnt[key]])
for key in three_inst_cnt.keys():
    three_inst_result.append([key, three_inst_cnt[key]])
one_inst_result.sort(key=lambda x: x[1])
two_inst_result.sort(key=lambda x: x[1])
three_inst_result.sort(key=lambda x: x[1])

one_inst_result.reverse()
two_inst_result.reverse()
three_inst_result.reverse()
data = [one_inst_result, two_inst_result, three_inst_result]
for j in range(3):
    current = data[j]
    print(j + 1)
    for i in range(10):
        print(current[i][0] + ": " + str(current[i][1] * 100 / len(lines)) + "%")
