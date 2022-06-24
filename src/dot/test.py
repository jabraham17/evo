
# def pair(i, j):
#     return 0.5*(i+j-2)*(i+j-1) + i

def pair(i, j):
    return 0.5*(i+j)*(i+j+1) + j

def get_i

seen = set()
for i in range(500):
    for j in range(500):
        v = pair(i, j)
        if v in seen:
            print(f"collision: {i} + {j} = {v}")
        seen.add(v)
