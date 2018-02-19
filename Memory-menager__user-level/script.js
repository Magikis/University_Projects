
const OPERATIONS = parseInt(process.argv[2])
const ALIGNS = 9;

function random(min,max)
{
    return Math.floor(Math.random()*(max-min+1)+min);
}
function ensureAllign(num) {
    const tab = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048];
    return   tab[num];
}


const data = [...Array(OPERATIONS).keys()].map(item => {
    return [
        random(0, 5),
        random(1, 3) != 1 ? random(1, 200) : random(1, 20000) ,
        ensureAllign(random(0, ALIGNS))
    ];
})
console.log (`#define MALLOCS_SIZE `, OPERATIONS,`

int data[MALLOCS_SIZE][3] = `);
console.log ('{')
data.forEach((item, index) => {
    console.log('{',item[0] ,', ', item[1],', ', item[2], '}', index < OPERATIONS -1 ? ',' : '');
})

console.log('};')


