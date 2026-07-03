document.addEventListener('DOMContentLoaded', function () {
  function handleSubmit(formId, statusId, type) {
    const form = document.getElementById(formId);
    if (!form) return;
    
    form.addEventListener('submit', function (e) {
      e.preventDefault();
      const status = document.getElementById(statusId);
      
      // Simple validation
      const data = new FormData(form);
      const entries = {};
      for (const [k, v] of data.entries()) entries[k] = v;
      
      // Simulate successful submit
      form.style.display = 'none';
      status.textContent = `Thank you — your ${type} form has been successfully submitted.`;
      status.classList.add('show');
    });
  }

  handleSubmit('volunteerForm', 'volunteerStatus', 'volunteer application');
  handleSubmit('communityForm', 'communityStatus', 'community join');
  handleSubmit('complaintForm', 'complaintStatus', 'complaint');
});
