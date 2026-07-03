const amountButtons = document.querySelectorAll('.amount-option');
const customAmount = document.getElementById('customAmount');
const summaryAmount = document.getElementById('summaryAmount');
const summaryMethod = document.getElementById('summaryMethod');
const paymentOptions = document.querySelectorAll('input[name="paymentMethod"]');
const donationForm = document.getElementById('donationForm');
const successPanel = document.getElementById('successPanel');
const impactGrid = document.querySelector('.impact-grid');

let selectedAmount = 100;

function formatAmount(value) {
  return `₹${Number(value).toLocaleString('en-IN')}`;
}

function updateSummary() {
  summaryAmount.textContent = formatAmount(selectedAmount || 0);
  const method = document.querySelector('input[name="paymentMethod"]:checked');
  summaryMethod.textContent = `Payment via ${method ? method.value : 'UPI'}`;
}

function setActiveAmount(button) {
  amountButtons.forEach((btn) => btn.classList.remove('active'));
  button.classList.add('active');
  customAmount.value = '';
  selectedAmount = Number(button.dataset.value);
  updateSummary();
}

amountButtons.forEach((button) => {
  button.addEventListener('click', () => {
    setActiveAmount(button);
  });
});

customAmount.addEventListener('input', () => {
  const value = Number(customAmount.value);
  if (value > 0) {
    amountButtons.forEach((btn) => btn.classList.remove('active'));
    selectedAmount = value;
    updateSummary();
  } else {
    const active = document.querySelector('.amount-option.active');
    selectedAmount = active ? Number(active.dataset.value) : 100;
    updateSummary();
  }
});

paymentOptions.forEach((option) => {
  option.addEventListener('change', updateSummary);
});

function animateCounters() {
  const counters = document.querySelectorAll('[data-target]');
  counters.forEach((counter) => {
    const target = Number(counter.dataset.target);
    let current = 0;
    const step = Math.max(1, Math.floor(target / 100));

    function update() {
      current += step;
      if (current > target) current = target;
      counter.textContent = current.toLocaleString('en-IN');
      if (current < target) {
        requestAnimationFrame(update);
      }
    }

    update();
  });
}

if (impactGrid) {
  const observer = new IntersectionObserver(
    (entries, obs) => {
      entries.forEach((entry) => {
        if (entry.isIntersecting) {
          animateCounters();
          obs.disconnect();
        }
      });
    },
    { threshold: 0.35 }
  );
  observer.observe(impactGrid);
}

if (donationForm) {
  donationForm.addEventListener('submit', (event) => {
    event.preventDefault();
    const name = document.getElementById('donorName').value.trim();
    const email = document.getElementById('donorEmail').value.trim();
    const phone = document.getElementById('donorPhone').value.trim();
    const payment = document.querySelector('input[name="paymentMethod"]:checked');

    if (!name || !email || !phone) {
      successPanel.textContent = 'Please complete the required fields before submitting.';
      successPanel.classList.add('visible');
      return;
    }

    successPanel.innerHTML = `
      <div class="success-message">
        <i class="fas fa-check-circle"></i>
        <div>
          <strong>Thank you, ${name.split(' ')[0]}!</strong>
          <p>Your donation of <strong>${formatAmount(selectedAmount)}</strong> via <strong>${payment ? payment.value : 'UPI'}</strong> is being processed. We will reach out to <strong>${email}</strong> with the next steps.</p>
        </div>
      </div>
    `;
    successPanel.classList.add('visible');
    donationForm.reset();
    setActiveAmount(document.querySelector('.amount-option[data-value="100"]'));
    document.querySelector('input[name="paymentMethod"][value="UPI"]').checked = true;
    updateSummary();
  });
}

updateSummary();
