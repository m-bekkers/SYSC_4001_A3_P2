import random

num_students = 20
student_nums = random.sample(range(0000, 9999), num_students)
student_nums.sort()

for i in range(num_students):
    student_nums[i] = str(student_nums[i])
    while len(student_nums[i]) < 4:
        student_nums[i] = "0" + student_nums[i]

    filename = "student_" + student_nums[i] + ".txt"
    with open("students/" + filename, "w") as f:
        f.write(student_nums[i])