const volunteerSearch = document.getElementById('volunteerSearch');
const roleFilter = document.getElementById('roleFilter');
const volunteerCards = document.querySelectorAll('.volunteer-card');
const counters = document.querySelectorAll('[data-target]');

function filterVolunteers() {
  const query = volunteerSearch.value.trim().toLowerCase();
  const role = roleFilter.value;

  volunteerCards.forEach((card) => {
    const name = card.dataset.name.toLowerCase();
    const cardRole = card.dataset.role;
    const matchesName = query === '' || name.includes(query);
    const matchesRole = role === 'all' || cardRole === role;
    card.style.display = matchesName && matchesRole ? 'grid' : 'none';
  });
}

function animateCounters() {
  counters.forEach((counter) => {
    const target = Number(counter.dataset.target);
    let current = 0;
    const increment = Math.ceil(target / 120);

    const update = () => {
      current += increment;
      if (current > target) current = target;
      counter.textContent = current.toLocaleString('en-IN');
      if (current < target) {
        requestAnimationFrame(update);
      }
    };

    update();
  });
}

if (volunteerSearch && roleFilter) {
  volunteerSearch.addEventListener('input', filterVolunteers);
  roleFilter.addEventListener('change', filterVolunteers);
}

if (counters.length) {
  const observer = new IntersectionObserver(
    (entries, obs) => {
      entries.forEach((entry) => {
        if (entry.isIntersecting) {
          animateCounters();
          obs.disconnect();
        }
      });
    },
    { threshold: 0.4 }
  );
  observer.observe(counters[0]);
}
