const express = require('express');
const router = express.Router();
var path = require('path');
var cors = require('cors');


var corsOptions = {
    origin: '*'
}

var cyclic = 2;

router.get("/set/:cycle", cors(corsOptions), async(req, res) => {
    try {
        cyclic = Number(req.params.cycle);
        res.json(req.params.cycle);
    } catch (err) {
        console.error("error: ", err);
        res.json(err);
    }
});

router.get("/get", cors(corsOptions), async(req, res) => {
    try {
        res.json(cyclic);
    } catch (err) {
        console.error("error: ", err);
        res.json(err);
    }
});

module.exports = router;