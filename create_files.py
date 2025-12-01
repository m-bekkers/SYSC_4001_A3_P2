import random

num_students = 19
student_nums = random.sample(range(0000, 9998), num_students)
student_nums.sort()

for i in range(num_students):
    student_nums[i] = str(student_nums[i])
    while len(student_nums[i]) < 4:
        student_nums[i] = "0" + student_nums[i]

    filename = "student_" + student_nums[i] + ".txt"
    with open("students/" + filename, "w") as f:
        f.write(student_nums[i])

with open("students/student_9999.txt", "w") as f:
    f.write("9999")