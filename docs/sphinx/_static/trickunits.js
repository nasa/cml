document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll("dt.sig.sig-object.cpp").forEach((dt) => {
        if (dt.textContent.trim() === "Units:") {
            dt.classList.add("trickunits-label");
            const dl = dt.closest("dl");
            if (dl) dl.classList.add("trickunits-block");
        }
    });
});
