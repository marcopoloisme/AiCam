import cv2

cap = cv2.VideoCapture(
"http://130.194.1.123:81/stream"
)

while True:

    ret, frame = cap.read()

    if not ret:
        continue

    gray = cv2.cvtColor(
        frame,
        cv2.COLOR_BGR2GRAY
    )

    edges = cv2.Canny(
        gray,
        100,
        200
    )

    cv2.imshow("Edges", edges)

    if cv2.waitKey(1) == ord('q'):
        break
