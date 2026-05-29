import tkinter as tk
import random
import string

def generate_password(length, use_uppercase, use_numbers, use_special):
    characters = string.ascii_lowercase  
    if use_uppercase:
        characters += string.ascii_uppercase  
    if use_numbers:
        characters += string.digits
    if use_special:
        characters += string.punctuation

    if length <= 0:
        return "Invalid length"

    password = ''.join(random.choice(characters) for _ in range(length))
    return password

def on_generate():
    length = int(length_entry.get())
    use_uppercase = uppercase_var.get()
    use_numbers = numbers_var.get()
    use_special = special_var.get()
   
    password = generate_password(length, use_uppercase, use_numbers, use_special)
    password_var.set(password)

root = tk.Tk()
root.title("Random Password Generator")

length_var = tk.IntVar(value=12)
uppercase_var = tk.BooleanVar(value=True)
numbers_var = tk.BooleanVar(value=True)
special_var = tk.BooleanVar(value=True)
password_var = tk.StringVar()

tk.Label(root, text="Password Length:").pack(pady=5)
length_entry = tk.Entry(root, textvariable=length_var)
length_entry.pack(pady=5)

tk.Checkbutton(root, text="Include Uppercase Letters", variable=uppercase_var).pack(pady=5)
tk.Checkbutton(root, text="Include Numbers", variable=numbers_var).pack(pady=5)
tk.Checkbutton(root, text="Include Special Characters", variable=special_var).pack(pady=5)

tk.Button(root, text="Generate Password", command=on_generate).pack(pady=10)

tk.Label(root, text="Generated Password:").pack(pady=5)
password_label = tk.Entry(root, textvariable=password_var, width=50)
password_label.pack(pady=5)

root.mainloop()
