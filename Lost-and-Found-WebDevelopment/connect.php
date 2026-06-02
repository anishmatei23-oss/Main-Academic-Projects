<?php
$email = $_POST['email'];
$password = $_POST['password'];

// Database connection
$con = new mysqli("localhost", "root", "", "dbms");

// Check connection
if ($con->connect_error) {
    die("Connection Failed: " . $con->connect_error);
}

$stmt = $con->prepare("SELECT * FROM project WHERE email = ?");
$stmt->bind_param("s", $email);
$stmt->execute();
$stmt_result = $stmt->get_result();

if ($stmt_result->num_rows > 0) {
    $data = $stmt_result->fetch_assoc();
    if ($data['password'] === $password) { // If passwords match
        echo "<script>
                alert('Login Successful!');
                window.location.href = 'front.html';
              </script>";
    } else {
        echo "<script>
                alert('Invalid Email or Password');
                window.location.href = 'home.html';
              </script>";
    }
} else {
    echo "<script>
            alert('Invalid Email or Password');
            window.location.href = 'home.html';
          </script>";
}

$stmt->close();
$con->close();
?>
