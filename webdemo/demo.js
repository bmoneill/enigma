const rotor1Element = document.getElementById("rotor1");
const rotor1IdxElement = document.getElementById("rotor1idx");
const rotor2Element = document.getElementById("rotor2");
const rotor2IdxElement = document.getElementById("rotor2idx");
const rotor3Element = document.getElementById("rotor3");
const rotor3IdxElement = document.getElementById("rotor3idx");
const plugboardElement = document.getElementById("plugboard");
const reflectorElement = document.getElementById("reflector");

const plaintextElement = document.getElementById("plaintext");
const outputElement = document.getElementById("ciphertext");
const outputDiv = document.getElementsByClassName("output")[0];

function populateRotorOptions() {
  const rotorOptions = [
    { value: 0, text: "Rotor I" },
    { value: 1, text: "Rotor II" },
    { value: 2, text: "Rotor III" },
    { value: 3, text: "Rotor IV" },
    { value: 4, text: "Rotor V" },
    { value: 5, text: "Rotor VI" },
    { value: 6, text: "Rotor VII" },
    { value: 7, text: "Rotor VIII" },
  ];

  var i = 2;
  ["rotor1", "rotor2", "rotor3"].forEach((element) => {
    rotorOptions.forEach((option) => {
      const optionElement = document.createElement("option");
      optionElement.value = option.value;
      optionElement.textContent = option.text;
      document.getElementById(element).appendChild(optionElement);
    });
    document.getElementById(element).value = i--;
  });
}

function populateReflectorOptions() {
  const reflectorSelect = document.getElementById("reflector");
  const reflectorOptions = [
    { value: 0, text: "UKW-A" },
    { value: 1, text: "UKW-B" },
    { value: 2, text: "UKW-C" },
  ];

  reflectorOptions.forEach((option) => {
    const optionElement = document.createElement("option");
    optionElement.value = option.value;
    optionElement.textContent = option.text;
    reflectorSelect.appendChild(optionElement);
  });

  reflectorSelect.value = 1;
}

function populateTaskOptions() {
  const taskSelect = document.getElementById("task");
  const taskOptions = [
    { value: 0, text: "Encrypt" },
    { value: 1, text: "Frequency Analysis" },
    { value: 2, text: "IoC Analysis" },
    { value: 3, text: "N-gram Analysis" },
  ];

  taskOptions.forEach((option) => {
    const optionElement = document.createElement("option");
    optionElement.value = option.value;
    optionElement.textContent = option.text;
    taskSelect.appendChild(optionElement);
  });

  taskSelect.value = 0;
}

function populateTaskArgumentOptions() {
  const taskArgumentSelect = document.getElementById("taskArgument");
  const taskArgumentOptions = [
    { value: 0, text: "No argument" },
    { value: 1, text: "Rotors" },
    { value: 2, text: "Rotor 0" },
    { value: 3, text: "Rotor 1" },
    { value: 4, text: "Rotor 2" },
    { value: 5, text: "Rotor Positions" },
    { value: 6, text: "Rotor 0 Position" },
    { value: 7, text: "Rotor 1 Position" },
    { value: 8, text: "Rotor 2 Position" },
    { value: 9, text: "Reflector" },
    { value: 10, text: "Plugboard" },
  ];

  taskArgumentOptions.forEach((option) => {
    const optionElement = document.createElement("option");
    optionElement.value = option.value;
    optionElement.textContent = option.text;
    taskArgumentSelect.appendChild(optionElement);
  });

  taskArgumentSelect.value = 0;
}

function sanitizeInput(input) {
  return input.toUpperCase().replace(/[^A-Z]/g, "");
}

function encrypt() {
  event.preventDefault();

  const enigma = enigmaAPI.initEnigma();
  const rotor1 = parseInt(rotor1Element.value);
  const rotor1Idx = parseInt(rotor1IdxElement.value);
  const rotor2 = parseInt(rotor2Element.value);
  const rotor2Idx = parseInt(rotor2IdxElement.value);
  const rotor3 = parseInt(rotor3Element.value);
  const rotor3Idx = parseInt(rotor3IdxElement.value);
  const reflector = parseInt(reflectorElement.value);
  const plugboard = plugboardElement.value;
  const plaintext = plaintextElement.value;

  Module._enigma_set_rotor(enigma, rotor1, 0);
  Module._enigma_set_rotor_index(enigma, 0, rotor1Idx);
  Module._enigma_set_rotor(enigma, rotor2, 1);
  Module._enigma_set_rotor_index(enigma, 1, rotor2Idx);
  Module._enigma_set_rotor(enigma, rotor3, 2);
  Module._enigma_set_rotor_index(enigma, 2, rotor3Idx);
  Module._enigma_set_reflector(enigma, reflector);
  enigmaAPI.setPlugboard(enigma, sanitizeInput(plugboard));

  const ciphertext = enigmaAPI.encodeString(enigma, sanitizeInput(plaintext));
  outputElement.value = ciphertext;
  outputDiv.style.visibility = "visible";
  Module._free(enigma);
}

populateRotorOptions();
populateReflectorOptions();
populateTaskOptions();
populateTaskArgumentOptions();

var form = document.getElementById("inputForm");
form.addEventListener("submit", encrypt);
