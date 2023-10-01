let compilationStandard = " "; // --std-c99 --std-c89, --std-c11
let compilationFlags = [];  // --opt-code-speed --allow-unsafe-read --opt-code-size
let compilationProcessor = " "; // -mmcs51 -mstm8, -mz80
let compilationProcessorFlag = " "; // --model-small --model-medium --model-large, --emit-externs --no-std-crt0 --oldralloc, --model-medium --model-large --out-fmt-elf
let file_id = 0; // currently opened file
let save_file = 0;

zalezneMCS51 = '<h3>Dostępne opcje dla procesora MCS51:</h3>\
                 <label class="container">--model-small\
                    <input type="radio" name="zalezne" value="--model-small">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--model-medium\
                    <input type="radio" name="zalezne" value="--model-medium">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--model-large\
                    <input type="radio" name="zalezne" value="--model-large">\
                    <span class="checkmark"></span>\
                 </label>\
                 <button class="save-btn" id="zalezne">Zapisz wybór</button>';

zalezneZ80 = '<h3>Dostępne opcje dla procesora Z80:</h3>\
                 <label class="container">--emit-externs\
                    <input type="radio" name="zalezne" value="--emit-externs">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--no-std-crt0\
                    <input type="radio" name="zalezne" value="--no-std-crt0">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--oldralloc\
                    <input type="radio" name="zalezne" value="--oldralloc">\
                    <span class="checkmark"></span>\
                 </label>\
                 <button class="save-btn" id="zalezne">Zapisz wybór</button>';

zalezneSTM8 = '<h3>Dostępne opcje dla procesora STM8:</h3>\
                 <label class="container">--model-medium\
                    <input type="radio" name="zalezne" value="--model-medium">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--model-large\
                    <input type="radio" name="zalezne" value="--model-large">\
                    <span class="checkmark"></span>\
                 </label>\
                 <label class="container">--out-fmt-elf\
                    <input type="radio" name="zalezne" value="--out-fmt-elf">\
                    <span class="checkmark"></span>\
                 </label>\
                 <button class="save-btn" id="zalezne">Zapisz wybór</button>';

/* ================ NAVBAR SCRIPTS ================ */

document.addEventListener("click", e => {
    const isDropdownButton = e.target.matches("[data-dropdown-button]")

    let currentDropdown
    if (isDropdownButton) {
        currentDropdown = e.target.closest("[data-dropdown]")
        currentDropdown.classList.toggle("active")
    }

    document.querySelectorAll("[data-dropdown].active").forEach(dropdown => {
        if (dropdown === currentDropdown) return
        dropdown.classList.remove("active")
    })
})

document.getElementById("save-compilation").addEventListener("click", function () {
    if (save_file === 0) {
        save_file = 1;
    } else {
        save_file = 0;
    }
});

/* ================ THEME SWITCH SWITCH ================ */

const themeButton = document.querySelector('#theme-button');
const currentTheme = localStorage.getItem('theme');

if (currentTheme) {
    document.documentElement.setAttribute('data-theme', currentTheme);

    if (currentTheme === 'light') {
        themeButton.textContent = 'Tryb ciemny';
    } else {
        themeButton.textContent = 'Tryb jasny';
    }
}

function switchTheme() {
    const currentTheme = document.documentElement.getAttribute('data-theme');
    if (currentTheme === 'light') {
        document.documentElement.setAttribute('data-theme', 'dark');
        localStorage.setItem('theme', 'dark');
        themeButton.textContent = 'Tryb jasny';
    } else {
        document.documentElement.setAttribute('data-theme', 'light');
        localStorage.setItem('theme', 'light');
        themeButton.textContent = 'Tryb ciemny';
    }
}

themeButton.addEventListener('click', switchTheme, false);

/* ================ TABS BUTTONS ================ */

const tabButtons = document.querySelectorAll('.tab-btn');
let lastClickedButton = null;

tabButtons.forEach(button => {
    button.addEventListener('click', () => {
        if (lastClickedButton) {
            lastClickedButton.classList.remove('clicked');
        }
        button.classList.add('clicked');
        lastClickedButton = button;
    });
});

function openTab(evt, tabName) {
    let i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tab-content");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
        tabcontent[i].classList.remove('active');
    }
    tablinks = document.getElementsByClassName("tab-btn");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].classList.remove('clicked');
    }
    document.getElementById(tabName).style.display = "block";
    document.getElementById(tabName).classList.add('active');
    evt.currentTarget.classList.add('clicked');
}

document.getElementById("defaultOpen").click();

document.getElementById("standard").addEventListener("click", function () {
    compilationStandard = document.querySelector('input[name="standard"]:checked').value;
});

document.getElementById("procesor").addEventListener("click", function () {
    compilationProcessor = document.querySelector('input[name="procesor"]:checked').value;
    compilationProcessorFlag = " ";
    let zalezneDiv = document.getElementById("ZALEŻNE");
    if (compilationProcessor === "-mmcs51") {
        zalezneDiv.innerHTML = zalezneMCS51;
    }
    if (compilationProcessor === "-mz80") {
        zalezneDiv.innerHTML = zalezneZ80;
    }
    if (compilationProcessor === "-mstm8") {
        zalezneDiv.innerHTML = zalezneSTM8;
    }
    document.getElementById("zalezne").addEventListener("click", function () {
        try {
            compilationProcessorFlag = document.querySelector('input[name="zalezne"]:checked').value;
        } catch (e) {
            compilationProcessorFlag = " ";
        }
    });
});

$(document).ready(function () {
    $('#optymalizacje').click(function () {
        compilationFlags = [];
        $('input[name="optymalizacje"]:checked').each(function () {
            compilationFlags.push($(this).val());
        });
    });
});

/* ================ CHOOSE FILE BUTTONS ================ */

const fileButtons = document.querySelectorAll('.choose-file-btn');
let lastClickedFile = null;

fileButtons.forEach(file => {
    file.addEventListener('click', () => {
        if (lastClickedFile) {
            lastClickedFile.classList.remove('clicked');
        }
        file.classList.add('clicked');
        lastClickedFile = file;
    });
});

/* ================ FILE CONTENT VIEW AND SAVE ================ */

$(document).ready(function () {
    $('.choose-file-btn').click(function () {
        file_id = $(this).data('id');
        $.ajax({
            type: 'GET',
            url: `/compiler/getfile/${file_id}/`,
            success: function (response) {
                $('#file-content').text(response.data);
            },
            error: function (response) {
                $('#file-content').text(response.data);
            }
        });
    });

    $('#save-file').click(function () {
        event.preventDefault();
        let csrf_token = $('input[name="csrfmiddlewaretoken"]').val();
        $.ajax({
            type: 'POST',
            url: `/compiler/savefile/${file_id}/`,
            data: {
                'file-content': $('#file-content').val(),
                'csrfmiddlewaretoken': csrf_token
            },
            success: function (response) {
                if (response.success) {
                    alert('Plik został zapisany!');
                } else {
                    alert('Wystąpił błąd podczas zapisywania pliku!');
                }
            },
            error: function (xhr, status, error) {
                alert('Wystąpił błąd podczas zapisywania pliku!');
            }
        });
    });
});

/* ================ FILE COMPILATION ================ */

$(document).ready(function () {
    $('#compile-button').click(function () {
        let compFlags = " ";
        for (let i = 0; i < compilationFlags.length; i++) {
            compFlags += compilationFlags[i] + " ";
        }
        $.ajax({
            type: 'GET',
            url: `/compiler/compilefile/${file_id}/${compilationStandard}/${compilationProcessor}/${compilationProcessorFlag}/${compFlags}/${save_file}`,
            success: function (response) {
                $('#compilation-result').text(response.result);
                save_file = 0;
            },
            error: function (response) {
                $('#compilation-result').text(response.result);
                save_file = 0;
            }
        });
    });
});
