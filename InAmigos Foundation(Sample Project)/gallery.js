document.addEventListener('DOMContentLoaded', function () {
  const modal = document.getElementById('galleryModal');
  const modalImage = document.getElementById('modalImage');
  const modalCaption = document.getElementById('modalCaption');
  const modalClose = document.querySelector('.modal-close');
  const modalPrev = document.querySelector('.modal-prev');
  const modalNext = document.querySelector('.modal-next');
  const expandButtons = document.querySelectorAll('.gallery-expand');
  const galleryItems = document.querySelectorAll('.gallery-item');

  let currentIndex = 0;
  const images = Array.from(galleryItems).map(item => ({
    img: item.querySelector('.gallery-image').src
  }));

  function openModal(index) {
    currentIndex = index;
    updateModal();
    modal.classList.add('active');
    document.body.style.overflow = 'hidden';
  }

  function closeModal() {
    modal.classList.remove('active');
    document.body.style.overflow = 'auto';
  }

  function updateModal() {
    const currentImage = images[currentIndex];
    modalImage.src = currentImage.img;
    modalCaption.textContent = '';
  }

  function nextImage() {
    currentIndex = (currentIndex + 1) % images.length;
    updateModal();
  }

  function prevImage() {
    currentIndex = (currentIndex - 1 + images.length) % images.length;
    updateModal();
  }

  expandButtons.forEach((button, index) => {
    button.addEventListener('click', () => openModal(index));
  });

  modalClose.addEventListener('click', closeModal);
  modalNext.addEventListener('click', nextImage);
  modalPrev.addEventListener('click', prevImage);

  // Close on background click
  modal.addEventListener('click', (e) => {
    if (e.target === modal) closeModal();
  });

  // Keyboard navigation
  document.addEventListener('keydown', (e) => {
    if (!modal.classList.contains('active')) return;
    if (e.key === 'Escape') closeModal();
    if (e.key === 'ArrowRight') nextImage();
    if (e.key === 'ArrowLeft') prevImage();
  });
});
