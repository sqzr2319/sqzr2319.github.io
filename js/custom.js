(() => {
  const reducedMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches
  if (!reducedMotion && 'IntersectionObserver' in window) {
    const observer = new IntersectionObserver(entries => {
      entries.forEach(entry => {
        if (entry.isIntersecting) {
          entry.target.classList.add('is-visible')
          observer.unobserve(entry.target)
        }
      })
    }, { threshold: 0.08 })
    document.querySelectorAll('.journal-entry, .sidebar-topics').forEach(element => {
      element.classList.add('reveal-entry')
      observer.observe(element)
    })
  }
  const menu = document.getElementById('navbarSupportedContent')
  document.addEventListener('keydown', event => {
    if (event.key === 'Escape' && menu.classList.contains('show')) {
      jQuery(menu).collapse('hide')
      document.getElementById('navbar-toggler-btn').focus()
    }
  })
  jQuery(menu).on('hidden.bs.collapse', () => {
    document.querySelector('.animated-icon').classList.remove('open')
    document.getElementById('navbar').classList.remove('navbar-col-show')
  })
})()
