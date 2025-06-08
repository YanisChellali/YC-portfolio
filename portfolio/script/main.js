let feminin = false;
function switchPodium() {
    feminin = !feminin;
    switch (feminin) {
        case true: // Podium féminin
            document.getElementById('btn').innerHTML = 'MASCULIN';
            document.getElementById('gold').src = '../img/sportifs/saya_sakakibara.jpg';
            document.getElementById('silver').src = '../img/sportifs/manon_veenstra.jpg';
            document.getElementById('bronze').src = '../img/sportifs/zoe_claessens.jpg';
            document.getElementById('gold').onclick = 'location.href = \'\'';
        break;
        case false: // Podium masculin
            document.getElementById('btn').innerHTML = 'FEMININ';
            document.getElementById('gold').src = '../img/sportifs/joris_daudet.jpg';
            document.getElementById('silver').src = '../img/sportifs/sylvain_andre.jpg';
            document.getElementById('bronze').src = '../img/sportifs/romain_mahieu.jpg';
            document.getElementById('gold').onclick = '';
        break;
    }
}

window.onscroll = function() {
    const nav = document.querySelector('nav');
    const sticky = nav.offsetTop;

    if (window.scrollY > sticky) {
        nav.classList.add('fixed');
    } else {
        nav.classList.remove('fixed');
    }
}

setInterval(() => {
    let size = innerWidth / 1920 * 49;
    document.documentElement.style.setProperty('--header-height', `${size}em`);

    if (innerWidth >= 953) {
        document.getElementById('burg').style.setProperty('display', 'none');
        isBurger = true;
        burgerisation();
    }
}, 1e2)

let isBurger = false;
let burgerWidth = 100;
function burgerisation() {
    isBurger = !isBurger;
    switch (isBurger) {
        case true:
            burgerWidth = 100;
            document.documentElement.style.setProperty('--burger-width', `${burgerWidth}%`);
            document.getElementById('nav').style.setProperty('border-bottom-right-radius', '0');
            document.getElementById('burg').style.setProperty('display', 'block');
            const interLess = setInterval(() => {
                burgerWidth -= 10;
                document.documentElement.style.setProperty('--burger-width', `${burgerWidth}%`);
                if (burgerWidth <= 0) {clearInterval(interLess); document.documentElement.style.setProperty('--burger-width', '0%');};
            }, 50);
        break;
        case false:
            burgerWidth = 0;
            document.documentElement.style.setProperty('--burger-width', `${burgerWidth}%`);
            document.getElementById('nav').style.setProperty('border-bottom-right-radius', '13px');
            document.getElementById('burg').style.setProperty('display', 'none');
            const interMore = setInterval(() => {
                burgerWidth += 10;
                document.documentElement.style.setProperty('--burger-width', `${burgerWidth}%`);
                if (burgerWidth >= 100) {clearInterval(interMore); document.documentElement.style.setProperty('--burger-width', '100%');};
            }, 50);
        break;
    }
}