const username = document.getElementById('username');
const login = document.getElementById('login');
const password = document.getElementById('password');
const usernamefeedback = document.querySelector('.username-invalid-feedback');
const loginfeedback = document.querySelector('.login-invalid-feedback');
const submitBtn = document.querySelector("#submit-btn");
let isUsernameValid = false;
let isLoginValid = false;

username.addEventListener("keyup", (e) => {
    const usernameVal = e.target.value;

    username.classList.remove("is-invalid");
    usernamefeedback.style.display = "none";

    if (usernameVal.length > 0) {
        fetch("/compiler/usernamevalidation/", {
            body: JSON.stringify({username: usernameVal}),
            method: "POST",
        })
            .then((res) => res.json())
            .then((data) => {
                if (data.username_error) {
                    isUsernameValid = false;
                    username.classList.add("is-invalid");
                    usernamefeedback.style.display = "block";
                    usernamefeedback.innerHTML = `<p style="color: red">${data.username_error}</ps>`;
                    submitBtn.disabled = true;
                } else {
                    isUsernameValid = true;
                    if (isLoginValid) {
                        submitBtn.removeAttribute("disabled");
                    }
                }
            });
    }
});

login.addEventListener("keyup", (e) => {
    const loginVal = e.target.value;

    login.classList.remove("is-invalid");
    loginfeedback.style.display = "none";

    if (loginVal.length > 0) {
        fetch("/compiler/loginvalidation/", {
            body: JSON.stringify({login: loginVal}),
            method: "POST",
        })
            .then((res) => res.json())
            .then((data) => {
                if (data.login_error) {
                    isLoginValid = false;
                    submitBtn.disabled = true;
                    login.classList.add("is-invalid");
                    loginfeedback.style.display = "block";
                    loginfeedback.innerHTML = `<p style="color: red">${data.login_error}</p>`;
                } else {
                    isLoginValid = true;
                    if (isUsernameValid) {
                        submitBtn.removeAttribute("disabled");
                    }
                }
            });
    }
});